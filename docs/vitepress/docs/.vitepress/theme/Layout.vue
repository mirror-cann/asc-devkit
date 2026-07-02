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
import DefaultTheme from 'vitepress/theme'
import FilterToggle from './FilterToggle.vue'
import BackToTop from './BackToTop.vue'
import CustomOutline from './CustomOutline.vue'
import ApiDownload from './ApiDownload.vue'
import EditOnGitcode from './EditOnGitcode.vue'
</script>

<template>
  <DefaultTheme.Layout>
    <template #nav-bar-content-after>
      <FilterToggle />
    </template>
    <template #aside-outline-before>
      <CustomOutline />
    </template>
    <template #doc-footer-before>
      <EditOnGitcode />
    </template>
    <template #layout-bottom>
      <ApiDownload />
      <BackToTop />
      <SidebarResizer />
    </template>
  </DefaultTheme.Layout>
</template>
