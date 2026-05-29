<script setup>
import { ref, computed } from 'vue'
import { useRoute } from 'vitepress'
import JSZip from 'jszip'

const route = useRoute()
const loading = ref(false)
const isApiPage = computed(() => route.path.startsWith('/api/'))

function getFilterVersion() {
  try {
    return localStorage.getItem('cann-filter-selection') || 'all'
  } catch {
    return 'all'
  }
}

function filterContentByVersion(content, version) {
  const htmlBlockRegex = /<p>\s*CANNFILTER_DIV_([A-Za-z0-9_,]+)_OPEN\s*<\/p>([\s\S]*?)<p>\s*CANNFILTER_DIV_\1_CLOSE\s*<\/p>/g
  const plainBlockRegex = /CANNFILTER_DIV_([A-Za-z0-9_,]+)_OPEN[ \t]*\n?([\s\S]*?)CANNFILTER_DIV_\1_CLOSE/g

  content = filterCannFilterTags(content, version)

  if (version === 'all') {
    content = content.replace(htmlBlockRegex, '$2')
    content = content.replace(plainBlockRegex, '$2')
  } else {
    content = content.replace(htmlBlockRegex, (match, ver, inner) => ver === version ? inner : '')
    content = content.replace(plainBlockRegex, (match, ver, inner) => ver === version ? inner : '')
  }

  content = content.replace(/<term\b[^>]*>([\s\S]*?)<\/term>/gi, '$1')
  content = content.replace(/<ph\b[^>]*>([\s\S]*?)<\/ph>/gi, '$1')
  content = content.replace(/<__gm__\b[^>]*>([\s\S]*?)<\/__gm__>/gi, '$1')
  content = content.replace(/<__ubuf__\b[^>]*>([\s\S]*?)<\/__ubuf__>/gi, '$1')
  return content
}

function filterCannFilterTags(src, version) {
  const tagRegex = /<(\/?)(cann-filter)((?:\s[^>]*)?)>/gi
  const stack = []
  const parts = []
  let lastIdx = 0
  let match
  while ((match = tagRegex.exec(src)) !== null) {
    const pos = match.index
    const isClose = match[1] === '/'
    const attrs = match[3]
    const before = src.slice(lastIdx, pos)
    const suppress = stack.length > 0 && stack[stack.length - 1]

    if (isClose) {
      if (stack.length > 0) {
        stack.pop()
      }
      if (!suppress) {
        const curSuppress = stack.length > 0 && stack[stack.length - 1]
        if (!curSuppress) parts.push(before)
      }
      lastIdx = pos + match[0].length
    } else {
      if (!suppress) parts.push(before)
      const attrMatch = attrs.match(/npu[_-]type\s*=\s*"([^"]+)"/i)
      if (attrMatch && version !== 'all') {
        const types = attrMatch[1].split(',').map(s => s.trim())
        const include = types.includes(version)
        stack.push(!include)
      } else {
        stack.push(false)
      }
      lastIdx = pos + match[0].length
    }
  }
  parts.push(src.slice(lastIdx))
  const result = parts.join('')
  return result.replace(/<\/?cann-filter\b[^>]*>/gi, '')
}

async function fetchManifest() {
  try {
    const resp = await fetch('/api-source/manifest.json')
    if (!resp.ok) return null
    return await resp.json()
  } catch {
    return null
  }
}

function getSidebarHrefs() {
  if (typeof document === 'undefined') return []
  const links = document.querySelectorAll('.VPSidebar a[href^="/api/"]')
  const hrefs = [...links].map(a => a.getAttribute('href'))
  return [...new Set(hrefs)]
}

function sidebarHrefToRelPath(href) {
  return href.replace(/^\/api\//, '').replace(/\.html$/, '').replace(/\/$/, '') + '.md'
}

function timestamp() {
  const now = new Date()
  const y = now.getFullYear()
  const m = String(now.getMonth() + 1).padStart(2, '0')
  const d = String(now.getDate()).padStart(2, '0')
  const h = String(now.getHours()).padStart(2, '0')
  const min = String(now.getMinutes()).padStart(2, '0')
  const s = String(now.getSeconds()).padStart(2, '0')
  return y + m + d + '-' + h + min + s
}

function downloadBlob(blob, filename) {
  const url = URL.createObjectURL(blob)
  const a = document.createElement('a')
  a.href = url
  a.download = filename
  document.body.appendChild(a)
  a.click()
  document.body.removeChild(a)
  URL.revokeObjectURL(url)
}

async function handleDownload() {
  const version = getFilterVersion()
  const versionLabel = version === 'all' ? 'all' : ('v' + version)

  loading.value = true

  try {
    const manifest = await fetchManifest()
    if (!manifest || Object.keys(manifest).length === 0) {
      console.error('No source files found in manifest')
      return
    }

    const sidebarHrefs = getSidebarHrefs()
    if (sidebarHrefs.length === 0) {
      console.error('No sidebar links found')
      return
    }

    const fileEntries = []
    for (const href of sidebarHrefs) {
      const relPath = sidebarHrefToRelPath(href)
      const decoded = decodeURIComponent(relPath).replace(/\\/g, '/')
      const hash = manifest[decoded]
      if (hash) {
        fileEntries.push({ relPath: decoded, hash })
      }
    }

    if (fileEntries.length === 0) {
      console.error('No matching source files found')
      return
    }

    const zip = new JSZip()
    const batchSize = 30

    for (let i = 0; i < fileEntries.length; i += batchSize) {
      const batch = fileEntries.slice(i, i + batchSize)
      const results = await Promise.all(batch.map(async ({ relPath, hash }) => {
        try {
          const url = '/api-source/files/' + hash + '.md'
          const resp = await fetch(url)
          if (!resp.ok) return null
          let content = await resp.text()
          content = filterContentByVersion(content, version)
          return { filename: relPath, content }
        } catch {
          return null
        }
      }))
      for (const r of results) {
        if (r) zip.file(r.filename, r.content)
      }
    }

    const blob = await zip.generateAsync({ type: 'blob' })
    const ts = timestamp()
    downloadBlob(blob, 'AscendC-API_' + versionLabel + '_' + ts + '.zip')
  } catch (err) {
    console.error('Download failed:', err)
  } finally {
    loading.value = false
  }
}
</script>

<template>
  <button
    v-if="isApiPage"
    class="api-download-btn"
    :class="{ loading }"
    :disabled="loading"
    :title="loading ? '打包中...' : '下载全部 API Markdown'"
    @click="handleDownload"
  >
    <svg
      v-if="!loading"
      width="20"
      height="20"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      stroke-width="2"
      stroke-linecap="round"
      stroke-linejoin="round"
    >
      <path d="M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4" />
      <polyline points="7 10 12 15 17 10" />
      <line x1="12" y1="15" x2="12" y2="3" />
    </svg>
    <span v-else class="spinner" />
  </button>
</template>

<style scoped>
.api-download-btn {
  position: fixed;
  bottom: 40px;
  right: 96px;
  width: 44px;
  height: 44px;
  border: 1px solid var(--vp-c-divider);
  border-radius: 50%;
  background: var(--vp-c-bg-soft);
  color: var(--vp-c-text-2);
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 99;
  transition: all 0.3s;
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1);
}

.api-download-btn:hover:not(:disabled) {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
}

.api-download-btn:disabled {
  cursor: wait;
  opacity: 0.8;
}

.api-download-btn.loading {
  border-color: var(--vp-c-brand-1);
  color: var(--vp-c-brand-1);
}

.spinner {
  width: 18px;
  height: 18px;
  border: 2px solid var(--vp-c-divider);
  border-top-color: var(--vp-c-brand-1);
  border-radius: 50%;
  animation: spin 0.8s linear infinite;
}

@keyframes spin {
  to { transform: rotate(360deg); }
}
</style>
