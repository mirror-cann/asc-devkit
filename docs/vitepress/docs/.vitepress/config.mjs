import { defineConfig } from 'vitepress'
import { existsSync, readFileSync } from 'node:fs'
import { join, resolve } from 'node:path'
import { load as cheerioLoad } from 'cheerio'
import { pagefindPlugin, chineseSearchOptimize } from 'vitepress-plugin-pagefind'
import sidebars from './sidebar.mjs'

const docsRoot = resolve(import.meta.dirname, '..')

function filterEmptyPages(items, docsRoot) {
  return items.reduce((acc, item) => {
    const entry = { ...item }
    if (item.items) {
      entry.items = filterEmptyPages(item.items, docsRoot)
    }
    if (item.items && entry.items.length > 0) {
      acc.push(entry)
      return acc
    }
    if (item.link) {
      const relPath = item.link.startsWith('/') ? item.link.slice(1) : item.link
      const headerPath = resolve(docsRoot, relPath + '.md.header')
      if (!existsSync(headerPath)) {
        acc.push(entry)
        return acc
      }
      try {
        const headers = JSON.parse(readFileSync(headerPath, 'utf-8'))
        if (headers.length === 0) return acc
      } catch {
        return acc
      }
    }
    acc.push(entry)
    return acc
  }, [])
}

const filteredSidebars = {}
for (const [key, items] of Object.entries(sidebars)) {
  filteredSidebars[key] = filterEmptyPages(items, docsRoot)
}

function extractBodyContent(html) {
  const bodyMatch = html.match(/<body[^>]*>([\s\S]*)<\/body>/i)
  if (bodyMatch) return bodyMatch[1]
  return html
}

function normalizeHtml(html) {
  const $ = cheerioLoad(html, {
    xml: {
      xmlMode: false,
      decodeEntities: false,
      withStartIndices: false,
      withEndIndices: false,
    },
  }, false)
  const result = $('body').length ? $('body').html() : $.html()
  if (result !== null && result !== undefined) {
    return balanceTags(result)
  }
  return ''
}

function loadRawHtml(htmlPath) {
  const content = readFileSync(htmlPath, 'utf-8')
  return '\n<!-- RAW_HTML -->\n' + normalizeHtml(extractBodyContent(content)) + '\n'
}

function balanceTags(html) {
  const VOID_TAGS = new Set([
    'area', 'base', 'br', 'col', 'embed', 'hr', 'img', 'input',
    'link', 'meta', 'param', 'source', 'track', 'wbr'
  ])
  const tagRegex = /<\/?([a-zA-Z][a-zA-Z0-9-]*)[^>]*>/g
  const stack = []
  const parts = []
  let lastIdx = 0
  let match
  while ((match = tagRegex.exec(html)) !== null) {
    const full = match[0]
    const tag = match[1].toLowerCase()
    const isClose = full.startsWith('</')
    const isSelfClose = full.endsWith('/>')
    const pos = match.index
    if (isClose) {
      if (stack.length > 0 && stack[stack.length - 1] === tag) {
        stack.pop()
      } else if (stack.includes(tag)) {
        let closeStr = ''
        while (stack.length > 0 && stack[stack.length - 1] !== tag) {
          closeStr += '</' + stack.pop() + '>'
        }
        stack.pop()
        parts.push(html.slice(lastIdx, pos))
        parts.push(closeStr)
        lastIdx = pos
      }
    } else if (!isSelfClose && !VOID_TAGS.has(tag)) {
      stack.push(tag)
    }
  }
  parts.push(html.slice(lastIdx))
  if (stack.length > 0) {
    let closeStr = ''
    for (let i = stack.length - 1; i >= 0; i--) {
      closeStr += '</' + stack[i] + '>'
    }
    parts.push(closeStr)
  }
  return parts.join('')
}

function htmlAsMdPlugin() {
  const root = resolve(import.meta.dirname, '..')

  return {
    name: 'html-as-md',
    enforce: 'pre',
    resolveId(id) {
      if (!id.endsWith('.md') || id.includes('?')) return null

      const absId = id.startsWith(root)
        ? id
        : id.startsWith('/')
          ? resolve(root, '.' + id)
          : resolve(root, id)

      const htmlPath = absId.replace(/\.md$/, '.html')
      if (!existsSync(htmlPath)) return null

      if (existsSync(absId)) return null
      return htmlPath + '?rawhtml=1'
    },
    load(id) {
      if (id.includes('?rawhtml=1')) {
        const htmlPath = id.split('?')[0]
        this.addWatchFile(htmlPath)
        return loadRawHtml(htmlPath)
      }

      if (!id.endsWith('.md') || id.includes('?')) return null
      const htmlPath = id.replace(/\.md$/, '.html')
      if (!existsSync(htmlPath)) return null

      this.addWatchFile(htmlPath)
      return loadRawHtml(htmlPath)
    },
  }
}

function placeholderPlugin() {
  const root = resolve(import.meta.dirname, '..')
  const placeholder = resolve(root, 'api', 'figures', 'placeholder.png')
  return {
    name: 'placeholder-images',
    resolveId(id, importer) {
      if (!importer || !id.match(/\.(png|jpg|jpeg|gif|svg|webp)$/i)) return
      let importerDir = resolve(importer, '..')
      if (importerDir.includes('?')) {
        importerDir = importerDir.split('?')[0]
      }
      const resolved = resolve(importerDir, id)
      if (!existsSync(resolved)) {
        return placeholder
      }
      return resolved
    },
  }
}

function replaceCannFilterTags(src, replacer) {
  const tagRegex = /<(\/?)(cann-filter)((?:\s[^>]*)?)>/gi
  const stack = []
  const parts = []
  let lastIdx = 0
  let match
  while ((match = tagRegex.exec(src)) !== null) {
    const pos = match.index
    const isClose = match[1] === '/'
    const attrs = match[3]
    parts.push(src.slice(lastIdx, pos))
    lastIdx = pos + match[0].length
    if (isClose) {
      if (stack.length > 0) {
        const opener = stack.pop()
        parts.push(replacer.end(opener.type, opener.contentStart, lastIdx))
      }
    } else {
      const attrMatch = attrs.match(/npu[_-]type\s*=\s*"([^"]+)"/i)
      if (attrMatch) {
        stack.push({ type: attrMatch[1], contentStart: lastIdx })
        parts.push(replacer.start(attrMatch[1]))
      }
    }
  }
  const end = src.slice(lastIdx)
  parts.push(end)
  return parts.join('')
}

function fixCannFilterTags(src) {
  if (!/<(cann-filter|term|ph|__gm__|__ubuf__)\b/i.test(src)) return src

  src = replaceCannFilterTags(src, {
    start(type) { return `\nCANNFILTER_DIV_${type}_OPEN\n` },
    end(type) { return `\nCANNFILTER_DIV_${type}_CLOSE\n` },
  })

  return src
    .replace(/<\/?cann-filter\b[^>]*>/gi, '')
    .replace(/<(term|ph|__gm__|__ubuf__)\b[^>]*>([\s\S]*?)<\/\1>/gis, '$2')
    .replace(/<(term|ph|__gm__|__ubuf__)\b[^>]*>/gi, '')
    .replace(/<\/(term|ph|__gm__|__ubuf__)>/gi, '')
}

function stripInnerTags(text) {
  return text
    .replace(/<a\s+name\s*=\s*"[^"]*"\s*><\/a>/gi, '')
    .replace(/<\/?span\b[^>]*>/gi, '')
    .replace(/<\/?p\b[^>]*>/gi, '')
    .replace(/<a\s+href\s*=\s*"([^"]*)"\s*>/gi, '<a href="$1">')
    .trim()
}

function convertHtmlTablesToMarkdown(src) {
  let result = src
  result = result.replace(
    /<table\b[\s\S]*?<\/table>/gi,
    (table) => {
      const theadMatch = table.match(/<thead\b[\s\S]*?<\/thead>/i)
      const tbodyMatches = []
      const tbodyRegex = /<tbody\b[\s\S]*?<\/tbody>/gi
      let m
      while ((m = tbodyRegex.exec(table)) !== null) {
        tbodyMatches.push(m[0])
      }

      const rows = []
      if (theadMatch) {
        const thsMatch = theadMatch[0].match(/<th\b[\s\S]*?<\/th>/gi)
        if (thsMatch) {
          rows.push(thsMatch.map(th => stripInnerTags(th.replace(/<th\b[^>]*>/i, '').replace(/<\/th>/i, ''))))
        }
      }

      for (const tbody of tbodyMatches) {
        const trsMatch = tbody.match(/<tr\b[\s\S]*?<\/tr>/gi)
        if (trsMatch) {
          for (const tr of trsMatch) {
            const tdsMatch = tr.match(/<td\b[\s\S]*?<\/td>/gi)
            if (tdsMatch) {
              const cells = tdsMatch.map(td => {
                let text = td.replace(/<td\b[^>]*>/i, '').replace(/<\/td>/i, '')
                text = stripInnerTags(text)
                const linkMatch = text.match(/<a href="([^"]*)">([^<]*)<\/a>/i)
                if (linkMatch) {
                  text = text.replace(linkMatch[0], '[' + linkMatch[2] + '](' + linkMatch[1] + ')')
                }
                return text
              })
              rows.push(cells)
            }
          }
        }
      }

      if (rows.length === 0) return table
      const colCount = Math.max(...rows.map(r => r.length))
      const aligned = rows.map(r => {
        while (r.length < colCount) r.push('')
        return r
      })

      const mdRows = aligned.map((r, i) => '| ' + r.join(' | ') + ' |')
      const sep = '| ' + Array(colCount).fill('---').join(' | ') + ' |'
      return '\n' + mdRows[0] + '\n' + sep + '\n' + mdRows.slice(1).join('\n') + '\n'
    }
  )

  result = result.replace(/<a\s+name\s*=\s*"[^"]*"\s*><\/a>/gi, '')
  result = result.replace(/<br\s*\/?>/gi, '\n')
  return result
}

function fixIndentedBlockquotes(src) {
  return src.replace(/^[ \t]{4,}(?=>)/gm, '')
}

function fixSpanCrossingTableCells(src) {
  let prev
  do {
    prev = src
    src = src.replace(
      /(<span[^>]*>)([\s\S]*?)\|([\s\S]*?)<\/span>/g,
      (match, open, beforePipe, afterPipe) =>
        open + beforePipe + '</span>|' + afterPipe
    )
  } while (src !== prev)
  return src
}

function escapeVueInterpolations(html) {
  return html.replace(
    /(<code\b[\s\S]*?<\/code>|<pre\b[\s\S]*?<\/pre>)|(\{\{[\s\S]*?\}\})/g,
    (match, codeBlock, templateExpr) => {
      if (codeBlock) return codeBlock
      return '<span v-pre>' + templateExpr + '</span>'
    }
  )
}

function addHeadingAnchors(html) {
  const headingRegex = /<h([23456])((?:\s[^>]*)?)>([\s\S]*?)<\/h\1>/gi
  const seen = new Map()
  return html.replace(headingRegex, (match, level, attrs, content) => {
    const text = content.replace(/<[^>]+>/g, '').trim()
    if (!text) return match
    const baseSlug = text.replace(/\s+/g, '-')
    const count = seen.get(baseSlug) || 0
    const slug = count === 0 ? baseSlug : `${baseSlug}-${count}`
    seen.set(baseSlug, count + 1)
    const idAttr = attrs.match(/\sid\s*=\s*["']/) ? '' : ` id="${slug}"`
    const anchor = `<a class="header-anchor" href="#${slug}">&#x200B;</a>`
    return `<h${level}${attrs}${idAttr}>${content}${anchor}</h${level}>`
  })
}

function loadHeaderCache(filePath) {
  const absPath = resolve(import.meta.dirname, '..', filePath)
  const cachePath = absPath + '.header'
  if (!existsSync(cachePath)) return null
  try {
    return JSON.parse(readFileSync(cachePath, 'utf-8'))
  } catch {
    return null
  }
}

export default defineConfig({
  mpa: false,
  srcExclude: ['public/api-source/**'],
  title: 'Ascend C',
  description: 'Ascend C 算子开发文档',
  lang: 'zh-CN',
  ignoreDeadLinks: true,
  lastUpdated: false,

  markdown: {
    config(md) {
      const originalRender = md.render.bind(md)
      const originalRenderAsync = md.renderAsync ? md.renderAsync.bind(md) : null
      const processSource = (src) => {
        src = fixCannFilterTags(src)
        if (src.startsWith('    >') || src.includes('\n    >')) {
          src = fixIndentedBlockquotes(src)
        }
        if (src.includes('<span') && src.includes('|')) {
          src = fixSpanCrossingTableCells(src)
        }
        if (src.includes('<table')) {
          src = convertHtmlTablesToMarkdown(src)
        }
        return src
      }
      const processHtml = (html) => {
        if (html.includes('CANNFILTER_DIV_')) {
          html = html.replace(/<p>\s*CANNFILTER_DIV_([A-Za-z0-9_,]+)_OPEN\s*<\/p>/g, '<div data-filter="$1">')
          html = html.replace(/<p>\s*CANNFILTER_DIV_([A-Za-z0-9_,]+)_CLOSE\s*<\/p>/g, '</div>')
        }
        if (html.includes('{{')) {
          html = escapeVueInterpolations(html)
        }
        return html
      }
      const renderFn = (src, env) => {
        if (src.includes('<!-- RAW_HTML -->')) {
          const htmlContent = src.replace(/^.*<!-- RAW_HTML -->\s*/s, '').replace(/\s*$/, '')
          let html = addHeadingAnchors(htmlContent)
          html = replaceCannFilterTags(html, {
            start(type) { return `<div data-filter="${type}">` },
            end() { return '</div>' },
          })
          html = escapeVueInterpolations(html)
          return `<div v-pre>\n${html}\n</div>`
        }
        src = processSource(src)
        let html = originalRender(src, env)
        html = processHtml(html)
        return `<div v-pre>${html}</div>`
      }
      md.render = function (src, env) {
        return renderFn(src, env)
      }
      if (originalRenderAsync) {
        md.renderAsync = async function (src, env) {
          return renderFn(src, env)
        }
      }
    },
  },

  transformPageData(pageData) {
    const cache = loadHeaderCache(pageData.filePath)
    if (cache && cache.length > 0) {
      pageData.outlineHeaders = cache
    }
  },

  buildConcurrency: 1,

  vite: {
    sourcemap: false,
    build: {
      rollupOptions: {
        external: ['/pagefind/pagefind.js'],
      },
    },
    resolve: {
      preserveSymlinks: true,
    },
    plugins: [htmlAsMdPlugin(), placeholderPlugin(), pagefindPlugin({
      btnPlaceholder: '搜索',
      placeholder: '搜索文档',
      emptyText: '未找到结果',
      heading: '共: {{searchResult}} 条结果',
      customSearchQuery: chineseSearchOptimize,
    }), {
      name: 'vitepress-override-search-vue',
      enforce: 'post',
      config: () => ({
        resolve: {
          alias: {
            './VPNavBarSearch.vue': resolve(import.meta.dirname, 'theme', 'Search.vue'),
          },
        },
      }),
    }],
    vue: {
      template: {
        compilerOptions: {
          onError: (error) => {
            if (error.code === 2) return
            throw error
          },
        },
      },
    },
  },

  themeConfig: {
    nav: [
      { text: '首页', link: '/' },
      { text: 'AscendC算子开发指南', link: '/guide/入门教程/Ascend-C概述与学习路径' },
      { text: 'Ascend C API', link: '/api/Ascend-C-API列表' },
    ],

    search: {
      provider: 'algolia',
    },

    sidebar: filteredSidebars,

    outline: false,

    docFooter: {
      prev: '上一页',
      next: '下一页',
    },

    footer: {
      message: '免责声明：本站内容由 <a href="https://gitcode.com/cann/asc-devkit/" target="_blank">asc-devkit</a> 仓 master 分支自动编译生成，属于持续开发版本，可能存在缺陷，仅供预览与参考。如需稳定及商用资料，请查阅官方 <a href="https://www.hiascend.com/zh/document" target="_blank">昇腾社区</a>。',	    
      copyright: 'Copyright Huawei Technologies Co., Ltd.',
    },
  },
})
