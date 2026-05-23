# SIMT编码任务目录说明

`experiment/simt_experiment`用于存放用户完成SIMT编码任务时新增的代码，避免直接修改公共样例目录`demo/simt`，也避免不同用户之间的任务代码路径互相冲突。

建议每个用户按“样例名_姓名拼音”的方式新建独立目录，例如：

```text
experiment/simt_experiment/basic_gather_zhangsan
experiment/simt_experiment/mixed_scatter_zhangsan
experiment/simt_experiment/fixed_shape_scatter_zhangsan
```

开发时可以从`demo/simt/basic_scatter`复制一份到自己的目录后再改写：

```bash
mkdir -p experiment/simt_experiment
cp -r demo/simt/basic_scatter experiment/simt_experiment/basic_gather_<姓名拼音>
```

提交代码前请确认只提交自己的任务目录，不要把其他用户目录或本地`build`编译产物一起提交。
