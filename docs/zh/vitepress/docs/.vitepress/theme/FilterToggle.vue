<!--
  Copyright (c) 2026 Huawei Technologies Co., Ltd.
  This program is free software, you can redistribute it and/or modify it under the terms and conditions of
  CANN Open Software License Agreement Version 2.0 (the "License").
  Please refer to the License for details. You may not use this file except in compliance with the License.
  THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE.
  See LICENSE in the root of the software repository for the full text of the License.
-->

<script setup>
import { ref, watch, onMounted, nextTick } from 'vue'
import { useRoute } from 'vitepress'

const route = useRoute()
const isApiPage = ref(false)

const filterOptions = [
  { value: 'all', label: '全量' },
  { value: '950', label: 'Ascend 950' },
  { value: 'A3', label: 'Atlas A3' },
  { value: '910b', label: 'Atlas A2' },
  { value: '910', label: 'Atlas 训练系列' },
  { value: '310p', label: 'Atlas 推理系列' },
  { value: '310b', label: 'Atlas 200I/500 A2' },
  { value: 'x90', label: 'Kirin X90' },
  { value: '9030', label: 'Kirin 9030' },
]

const selectedFilter = ref('all')

function loadSavedFilter() {
  try {
    const saved = localStorage.getItem('cann-filter-selection')
    if (saved && filterOptions.some(o => o.value === saved)) {
      selectedFilter.value = saved
    }
  } catch {}
}

function applyFilter() {
  if (typeof document === 'undefined') return

  const content = document.querySelector('.vp-doc')
  if (!content) return

  const allFilterDivs = content.querySelectorAll('[data-filter]')
  content.querySelectorAll('tr').forEach(tr => { tr.style.display = '' })

  if (selectedFilter.value === 'all') {
    allFilterDivs.forEach(el => { el.style.display = '' })
  } else {
    allFilterDivs.forEach(el => {
      if (el.tagName === 'TR') return
      const filterValue = el.getAttribute('data-filter')
      const match = filterValue.split(',').map(s => s.trim()).includes(selectedFilter.value)
      el.style.display = match ? '' : 'none'
    })
    content.querySelectorAll('tr').forEach(tr => {
      if (tr.hasAttribute('data-filter')) {
        const filterValue = tr.getAttribute('data-filter')
        const match = filterValue.split(',').map(s => s.trim()).includes(selectedFilter.value)
        tr.style.display = match ? '' : 'none'
        return
      }
      const filterDivs = tr.querySelectorAll('[data-filter]')
      if (filterDivs.length === 0) return
      const anyVisible = Array.from(filterDivs).some(el => el.style.display !== 'none')
      tr.style.display = anyVisible ? '' : 'none'
    })
  }
}

function onFilterChange() {
  localStorage.setItem('cann-filter-selection', selectedFilter.value)
  applyFilter()
}

onMounted(async () => {
  loadSavedFilter()
  if (route.path.startsWith('/api/')) {
    isApiPage.value = true
  }
  await nextTick()
  setTimeout(applyFilter, 100)
})

watch(() => route.path, async (path) => {
  isApiPage.value = path.startsWith('/api/')
  if (isApiPage.value) {
    await nextTick()
    setTimeout(applyFilter, 200)
  }
}, { immediate: true })
</script>

<template>
  <div class="cann-filter-bar" :class="{ hidden: !isApiPage }">
    <span class="filter-label">版本</span>
    <select v-model="selectedFilter" class="filter-select" :disabled="!isApiPage" @change="onFilterChange">
      <option v-for="opt in filterOptions" :key="opt.value" :value="opt.value">
        {{ opt.label }}
      </option>
    </select>
  </div>
</template>
