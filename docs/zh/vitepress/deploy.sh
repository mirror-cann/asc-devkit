#!/bin/bash
# ----------------------------------------------------------------------------------------------------------
# Copyright (c) 2026 Huawei Technologies Co., Ltd.
# This program is free software, you can redistribute it and/or modify it under the terms and conditions of
# CANN Open Software License Agreement Version 2.0 (the "License").
# Please refer to the License for details. You may not use this file except in compliance with the License.
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
# See LICENSE in the root of the software repository for the full text of the License.
# ----------------------------------------------------------------------------------------------------------

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
LOG_FILE="$SCRIPT_DIR/deploy.log"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

log()  { echo -e "${GREEN}[INFO]${NC} $(date '+%H:%M:%S') $*" | tee -a "$LOG_FILE"; }
warn() { echo -e "${YELLOW}[WARN]${NC} $(date '+%H:%M:%S') $*" | tee -a "$LOG_FILE"; }
err()  { echo -e "${RED}[ERROR]${NC} $(date '+%H:%M:%S') $*" | tee -a "$LOG_FILE"; }

# ============================================================
# 参数解析
# ============================================================
MODE="build"
NGINX_PORT=80
INSTALL_NODE=false
SERVE_PORT=38090
while [[ $# -gt 0 ]]; do
    case "$1" in
        --nginx)    MODE="nginx"; shift ;;
        --port)     NGINX_PORT="$2"; shift 2 ;;
        --preview)  MODE="preview"; shift ;;
        --serve)    MODE="serve"; SERVE_PORT="$2"; shift 2 ;;
        --help|-h)
            echo "用法: $0 [选项]"
            echo ""
            echo "选项:"
            echo "  (无参数)        构建静态站点到 docs/.vitepress/dist"
            echo "  --nginx         构建并配置 Nginx，注册为 systemd 开机自启服务"
            echo "  --port <端口>   Nginx 监听端口 (默认 80)"
            echo "  --preview       构建并以预览模式启动 (开发用)"
            echo "  --serve <端口>   构建并在指定端口提供静态服务 (默认 38090)"
            echo "  --help, -h      显示帮助"
            exit 0
            ;;
        *)          err "未知参数: $1"; exit 1 ;;
    esac
done

> "$LOG_FILE"

# ============================================================
# 0. 系统检测
# ============================================================
detect_os() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS=$ID
        OS_VERSION=$VERSION_ID
    elif [ -f /etc/redhat-release ]; then
        OS="centos"
        OS_VERSION=$(rpm -q --qf "%{VERSION}" $(rpm -q --whatprovides redhat-release) 2>/dev/null || echo "7")
    else
        OS="unknown"
        OS_VERSION="unknown"
    fi
    log "检测到系统: $OS $OS_VERSION"
}

# ============================================================
# 1. 安装 Node.js (>=18)
# ============================================================
install_nodejs() {
    if command -v node &>/dev/null; then
        NODE_VER=$(node -v | sed 's/v//' | cut -d. -f1)
        if [ "$NODE_VER" -ge 18 ]; then
            log "Node.js $(node -v) 已满足要求 (>=18)"
            return 0
        fi
        warn "当前 Node.js $(node -v) 版本过低，需要 >=18，正在升级..."
    fi

    log "安装 Node.js 18 LTS..."

    case "$OS" in
        ubuntu|debian)
            log "使用 NodeSource 安装 Node.js 18..."
            if ! command -v curl &>/dev/null; then
                apt-get update -y && apt-get install -y curl gnupg
            fi
            curl -fsSL https://deb.nodesource.com/setup_18.x | bash -
            apt-get install -y nodejs
            ;;

        centos|rhel|fedora|tencentos|anolis|openEuler)
            log "使用 NodeSource 安装 Node.js 18..."
            if ! command -v curl &>/dev/null; then
                yum install -y curl
            fi
            curl -fsSL https://rpm.nodesource.com/setup_18.x | bash -
            yum install -y nodejs
            ;;

        *)
            err "不支持的系统: $OS，请手动安装 Node.js >=18"
            exit 1
            ;;
    esac

    log "Node.js 安装完成: $(node -v)"
    log "npm 版本: $(npm -v)"
}

# ============================================================
# 2. 安装项目依赖
# ============================================================
install_deps() {
    cd "$SCRIPT_DIR"

    if [ ! -f "package.json" ]; then
        err "未找到 package.json，请在 vitepress 目录下执行本脚本"
        exit 1
    fi

    if [ ! -d "node_modules" ]; then
        log "安装项目依赖..."
        if command -v pnpm &>/dev/null; then
            pnpm install 2>&1 | tee -a "$LOG_FILE"
        else
            log "pnpm 未安装，使用 npm..."
            npm install --legacy-peer-deps 2>&1 | tee -a "$LOG_FILE"
        fi
        log "依赖安装完成"
    else
        log "依赖已存在，跳过安装"
    fi
}

# ============================================================
# 3. 安装 Python 依赖
# ============================================================
install_python_deps() {
    log "检查 Python 依赖 (cmarkgfm, pygments)..."

    local missing=0
    if ! python3 -c "import cmarkgfm" &>/dev/null; then missing=1; fi
    if ! python3 -c "import pygments" &>/dev/null; then missing=1; fi

    if [ "$missing" -eq 0 ]; then
        log "Python 依赖已满足"
        return 0
    fi

    log "安装 Python 依赖..."
    cd "$SCRIPT_DIR"

    local pip_cmd=""
    if command -v pip3 &>/dev/null; then
        pip_cmd="pip3"
    elif command -v pip &>/dev/null; then
        pip_cmd="pip"
    else
        err "未找到 pip，请手动安装: pip install -r requirements.txt"
        exit 1
    fi

    $pip_cmd install -r requirements.txt --break-system-packages -i https://mirrors.huaweicloud.com/repository/pypi/simple 2>&1 | tee -a "$LOG_FILE"
    log "Python 依赖安装完成"
}

# ============================================================
# 4. 构建静态站点
# ============================================================
build_docs() {
    cd "$SCRIPT_DIR"

    log "开始构建 VitePress 站点..."
    npm run docs:prebuild 2>&1 | tee -a "$LOG_FILE"
    npx cross-env NODE_OPTIONS=--max-old-space-size=14336 vitepress build docs 2>&1 | tee -a "$LOG_FILE"

    DIST_DIR="$SCRIPT_DIR/docs/.vitepress/dist"
    if [ ! -d "$DIST_DIR" ]; then
        err "构建失败: 未生成 $DIST_DIR"
        exit 1
    fi
    log "构建成功! 产物目录: $DIST_DIR"
}

# ============================================================
# 5. Nginx 部署
# ============================================================
deploy_nginx() {
    DIST_DIR="$SCRIPT_DIR/docs/.vitepress/dist"
    NGINX_ROOT="/usr/share/nginx/html/ascendc-docs"
    NGINX_CONF="/etc/nginx/conf.d/ascendc-docs.conf"

    log "=== Nginx 部署模式 ==="

    # 安装 Nginx
    if ! command -v nginx &>/dev/null; then
        log "安装 Nginx..."
        case "$OS" in
            ubuntu|debian)     apt-get update -y && apt-get install -y nginx ;;
            centos|rhel|fedora|tencentos|anolis|openEuler)
                yum install -y epel-release 2>/dev/null || true
                yum install -y nginx
                ;;
            *)                 err "请手动安装 Nginx"; exit 1 ;;
        esac
    fi

    # 拷贝构建产物
    log "拷贝构建产物到 $NGINX_ROOT"
    mkdir -p "$NGINX_ROOT"
    rm -rf "$NGINX_ROOT"/*
    cp -r "$DIST_DIR"/* "$NGINX_ROOT/"
    log "文件拷贝完成, 共 $(find "$NGINX_ROOT" -type f | wc -l) 个文件"

    # 生成 Nginx 配置
    log "生成 Nginx 配置: $NGINX_CONF"
    cat > "$NGINX_CONF" << NGINX_EOF
# Ascend C Documentation - VitePress Static Site
server {
    listen       ${NGINX_PORT};
    server_name  _;
    root         ${NGINX_ROOT};
    index        index.html;

    charset utf-8;

    # 日志
    access_log  /var/log/nginx/ascendc-docs-access.log;
    error_log   /var/log/nginx/ascendc-docs-error.log;

    # MPA 路由支持 - VitePress 多页应用需要精确匹配 .html
    location / {
        try_files \$uri \$uri.html \$uri/ \$uri/index.html =404;
    }

    # 静态资源缓存
    location ~* \.(js|css|png|jpg|jpeg|gif|ico|svg|woff|woff2|ttf|eot)$ {
        expires 30d;
        add_header Cache-Control "public, immutable";
    }

    # 禁止访问隐藏文件
    location ~ /\. {
        deny all;
    }

    # Gzip 压缩
    gzip on;
    gzip_types text/plain text/css application/json application/javascript text/xml application/xml text/javascript image/svg+xml;
    gzip_min_length 1000;
}
NGINX_EOF

    # 测试配置
    nginx -t 2>&1 | tee -a "$LOG_FILE"
    if ! nginx -t &>/dev/null; then
        err "Nginx 配置测试失败，请检查配置"
        exit 1
    fi

    # 启动/重载 Nginx
    systemctl enable nginx 2>/dev/null || true
    systemctl restart nginx 2>/dev/null || service nginx restart 2>/dev/null || nginx -s reload

    # 检查服务状态
    sleep 1
    if curl -s -o /dev/null -w "%{http_code}" "http://127.0.0.1:${NGINX_PORT}/" | grep -q "200\|301\|302"; then
        log "部署成功! 访问地址: http://$(hostname -I 2>/dev/null | awk '{print $1}' || echo '127.0.0.1'):${NGINX_PORT}"
    else
        warn "Nginx 可能未正常启动，请手动检查"
    fi
}

# ============================================================
# 6. 预览模式
# ============================================================
run_preview() {
    cd "$SCRIPT_DIR"
    log "启动预览模式 (Ctrl+C 退出)..."
    if command -v pnpm &>/dev/null; then
        pnpm docs:preview --port "$SERVE_PORT" --host 0.0.0.0
    else
        npm run docs:preview -- --port "$SERVE_PORT" --host 0.0.0.0
    fi
}

# ============================================================
# 7. 静态服务模式 (使用 http-server 或 python)
# ============================================================
run_serve() {
    cd "$SCRIPT_DIR"
    DIST_DIR="docs/.vitepress/dist"

    if [ ! -d "$DIST_DIR" ]; then
        err "请先构建: $0"
        exit 1
    fi

    log "在 ${SERVE_PORT} 端口提供静态服务..."

    if command -v npx &>/dev/null; then
        log "使用 http-server 提供服务..."
        npx http-server "$DIST_DIR" -p "$SERVE_PORT" -c-1 --gzip -a 0.0.0.0
    elif command -v python3 &>/dev/null; then
        log "使用 python3 提供服务..."
        python3 -m http.server "$SERVE_PORT" --directory "$DIST_DIR" --bind 0.0.0.0
    else
        err "未找到 http-server 或 python3"
        exit 1
    fi
}

# ============================================================
# 主流程
# ============================================================
detect_os

# 需要 root 权限的场景
case "$MODE" in
    nginx)
        if [ "$(id -u)" -ne 0 ]; then
            err "Nginx 部署需要 root 权限，请使用 sudo 执行"
            exit 1
        fi
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        deploy_nginx
        ;;
    build)
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        log "构建完成! 使用以下命令启动服务:"
        log "  sudo $0 --nginx        # Nginx 生产部署"
        log "  $0 --serve          # 临时静态服务 (端口 $SERVE_PORT)"
        log "  $0 --serve 8080     # 指定端口"
        log "  $0 --preview        # VitePress 预览模式"
        ;;
    preview)
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        run_preview
        ;;
    serve)
        install_nodejs
        install_deps
        install_python_deps
        build_docs
        run_serve
        ;;
esac
