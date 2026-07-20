/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 * This program is free software, you can redistribute it and/or modify it under the terms and conditions of
 * CANN Open Software License Agreement Version 2.0 (the "License").
 * Please refer to the License for details. You may not use this file except in compliance with the License.
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
 * See LICENSE in the root of the software repository for the full text of the License.
 */

import { readFileSync, writeFileSync, readdirSync, statSync } from 'node:fs'
import { resolve, join, relative } from 'node:path'

const MAX_TEXT_LENGTH = 2000
const CJK_REGEX = /[\u4e00-\u9fff\u3400-\u4dbf\uf900-\ufaff]/

function walkDir(dir) {
  const results = []
  for (const entry of readdirSync(dir, { withFileTypes: true })) {
    const fullPath = join(dir, entry.name)
    if (entry.isDirectory()) {
      results.push(...walkDir(fullPath))
    } else if (entry.isFile() && entry.name.endsWith('.html')) {
      results.push(fullPath)
    }
  }
  return results
}

function stripTags(html) {
  let text = html
  text = text.replace(/<script\b[\s\S]*?<\/script>/gi, '')
  text = text.replace(/<style\b[\s\S]*?<\/style>/gi, '')
  text = text.replace(/<noscript\b[\s\S]*?<\/noscript>/gi, '')
  text = text.replace(/<nav\b[\s\S]*?<\/nav>/gi, '')
  text = text.replace(/<footer\b[\s\S]*?<\/footer>/gi, '')
  text = text.replace(/<header\b[\s\S]*?<\/header>/gi, '')
  text = text.replace(/<[^>]+>/g, ' ')
  text = text.replace(/&\w+;/g, ' ')
  text = text.replace(/\s+/g, ' ').trim()
  return text
}

function extractSearchableText(html) {
  const bodyMatch = html.match(/<body[^>]*data-pagefind-body[^>]*>([\s\S]*)<\/body>/i)
  if (!bodyMatch) return ''
  let body = bodyMatch[1]
  body = body.replace(/<[^>]*data-pagefind-ignore[^>]*>[\s\S]*?<\/\w+>/gi, '')
  return stripTags(body).substring(0, MAX_TEXT_LENGTH)
}

function extractTitle(html, url) {
  const h1Match = html.match(/<h1[^>]*>([\s\S]*?)<\/h1>/i)
  if (h1Match) {
    const t = stripTags(h1Match[1]).trim()
    if (t && t.length < 100) return t
  }
  const segments = url.split('/').filter(Boolean).map(decodeURIComponent)
  const last = segments[segments.length - 1] || ''
  const secondLast = segments.length >= 2 ? segments[segments.length - 2] : ''
  if (secondLast && last && secondLast !== last) {
    const cjkMatch = secondLast.match(/[\u4e00-\u9fff\u3400-\u4dbf\uf900-\ufaff]/)
    if (cjkMatch) return secondLast + ' > ' + last
  }
  return last || secondLast
}

function extractHeadings(html, title) {
  const headings = [[1, title]]
  const bodyMatch = html.match(/<body[^>]*data-pagefind-body[^>]*>([\s\S]*)<\/body>/i)
  if (!bodyMatch) return headings
  const re = /<h([2-6])[^>]*id="([^"]*)"[^>]*>([\s\S]*?)<\/h\1>/gi
  let m
  while ((m = re.exec(bodyMatch[1])) !== null) {
    const level = parseInt(m[1])
    const text = stripTags(m[3]).trim()
    if (text && text.length < 200) {
      headings.push([level, text])
    }
  }
  return headings
}

function buildIndex(distDir) {
  const htmlFiles = walkDir(distDir)
  console.log(`[cjk-index] Found ${htmlFiles.length} HTML files`)

  const index = []
  let skipped = 0

  for (const filePath of htmlFiles) {
    try {
      const html = readFileSync(filePath, 'utf-8')
      if (!html.includes('data-pagefind-body')) {
        skipped++
        continue
      }

      const relPath = relative(distDir, filePath).replace(/\\/g, '/')
      const url = '/' + relPath.replace(/\.html$/, '')
      const title = extractTitle(html, url)
      const text = extractSearchableText(html)

      if (!text || !CJK_REGEX.test(text)) {
        skipped++
        continue
      }

      const headings = extractHeadings(html, title)
      index.push([url, title, text, headings])
    } catch (e) {
      // skip unreadable files
    }
  }

  console.log(`[cjk-index] Indexed ${index.length} pages, skipped ${skipped}`)

  const outputPath = resolve(distDir, 'search-cjk-index.json')
  writeFileSync(outputPath, JSON.stringify(index), 'utf-8')

  const sizeKB = Math.round(statSync(outputPath).size / 1024)
  console.log(`[cjk-index] Output: ${outputPath} (${sizeKB} KB)`)
}

const distDir = process.argv[2]
if (!distDir) {
  console.error('Usage: node build-cjk-index.mjs <distDir>')
  process.exit(1)
}
buildIndex(distDir)
