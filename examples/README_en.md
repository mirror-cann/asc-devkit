# Example Execution Verification

After developers implement custom operators using Ascend C API, they can verify operator functionality through single-operator invocation. This repository provides some operator implementations and their invocation examples, as shown below.

## Example List
|  Directory Name                                                   |  Function Description                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| [01_simd_cpp_api](./01_simd_cpp_api) | SIMD API examples based on Ascend C, introducing SIMD API usage through <<<>>> direct invocation implementation |
| [02_simd_c_api](./02_simd_c_api) | C_API examples based on Ascend C, introducing C_API usage through C_API implementation |
| [03_simt_api](./03_simt_api) | Operator examples based on Ascend C SIMT programming, introducing SIMT usage through <<<>>> direct invocation implementation |
| [04_aicpu](./04_aicpu) | Operator examples based on Ascend C AICPU programming, introducing AICPU usage through <<<>>> direct invocation implementation |
| [05_simd_simt_hybrid](./05_simd_simt_hybrid) | SIMD and SIMT hybrid programming samples, introducing introductory and high-performance optimization examples |
## npu-arch Build Option Description

Developers need to modify the --npu-arch build option in the CMakeLists.txt file under the specific example directory according to the actual execution environment. Refer to the correspondence in the table below to modify the npu-arch parameter value for the environment.
| Product Model |  npu-arch Parameter |
| ---- | ---- |
| Ascend 950PR/Ascend 950DT | --npu-arch=dav-3510 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products<br>Atlas A2 Training Series Products/Atlas A2 Inference Series Products | --npu-arch=dav-2201 |
| Atlas Inference Series Products AI Core | --npu-arch=dav-2002 |
