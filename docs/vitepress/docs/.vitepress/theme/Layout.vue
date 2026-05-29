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
    </template>
  </DefaultTheme.Layout>
</template>
