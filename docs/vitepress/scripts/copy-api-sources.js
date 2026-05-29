/**
 * Copyright (c) 2026 Huawei Technologies Co., Ltd.
 */

const fs = require('fs')
const path = require('path')
const crypto = require('crypto')

const root = path.resolve(__dirname, '..')
const repoRoot = path.resolve(root, '..', '..')
const apiDir = path.resolve(repoRoot, 'docs', 'api')
const outDir = path.resolve(root, 'docs', 'public', 'api-source')

function walkDir(dir, entries) {
  const list = fs.readdirSync(dir)
  for (const name of list) {
    const full = path.join(dir, name)
    const stat = fs.statSync(full)
    if (stat.isDirectory()) {
      walkDir(full, entries)
    } else if (stat.isFile() && name.endsWith('.md')) {
      entries.push(full)
    }
  }
}

function hashPath(str) {
  return crypto.createHash('md5').update(str).digest('hex')
}

if (!fs.existsSync(apiDir)) {
  console.error('api dir not found:', apiDir)
  process.exit(1)
}

fs.mkdirSync(outDir, { recursive: true })

const files = []
walkDir(apiDir, files)

const flatDir = path.join(outDir, 'files')
fs.mkdirSync(flatDir, { recursive: true })

const manifest = {}
for (const src of files) {
  const rel = path.relative(apiDir, src).replace(/\\/g, '/')
  const hash = hashPath(rel)
  manifest[rel] = hash
  const dest = path.join(flatDir, hash + '.md')
  fs.copyFileSync(src, dest)
}

fs.writeFileSync(path.join(outDir, 'manifest.json'), JSON.stringify(manifest, null, 2), 'utf-8')
console.log(`copied ${files.length} .md files to docs/public/api-source/files/`)
