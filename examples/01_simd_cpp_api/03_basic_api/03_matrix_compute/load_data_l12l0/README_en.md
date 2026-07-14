# load_data_l12l0 Example

## Overview

This example demonstrates the complete flow for B4 / B8 / B16 / B32 input data types, covering A / B matrix transfer from GM to L1, L1 to L0, matrix multiplication, and result transfer out.

The example covers A / B matrix transposed and non-transposed input combinations, focusing on the usage of `LoadData`, `LoadDataWithTranspose`, `Mmad`, and `Fixpipe` interfaces.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── load_data_l12l0
│   ├── figures                     // Illustrations
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── load_data_l12l0.asc         // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

### 1. Overall Flow

This example uses a fixed matrix multiplication specification of `[m, n, k] = [40, 50, 70]`, with output C matrix `[40, 50]` in ND format. The kernel function name is `KernelLoadDataL12L0`, selecting different scenarios through `scenarioNum`.

A complete matrix multiplication includes four phases: GM -> L1, L1 -> L0A / L0B, matrix multiplication, and L0C -> GM, as shown below:

```text
GM(ND) -> L1(Nz/Zz) -> L0A(Zz)/L0B(Zn) -> L0C(Zz) -> GM(ND)
       │            │                       │          │
    DataCopy     LoadData                 Mmad       Fixpipe
```

<div align="center">
  <img src="figures/cube.png" width="900"><br>
  Figure 1: Matrix multiplication overall flow diagram
</div>

Each phase processes the following:

1. **GM -> L1**: Call `DataCopy` interface with `Nd2NzParams` structure parameters to convert A / B matrices from ND layout to Nz or Zz layout.
2. **L1 -> L0A / L0B**: Based on data type, transpose flags, and L1 layout, select `LoadData (LoadData2DParams)`, `LoadDataWithTranspose`, or `LoadData (LoadData3DParamsV2)` to complete transfer and format transformation. For convenience, these are abbreviated as `Load2D`, `LoadDataWithTranspose`, and `Load3Dv2` respectively below.
3. **Matrix multiplication**: Call `Mmad` interface to perform matrix multiplication.
4. **L0C -> GM**: Call `Fixpipe` interface to transfer result C matrix out to GM.

The data layout formats of A / B matrices in different storage units are shown in [Table 1](#table1). L1 uses Nz layout in regular scenarios; in scenario 14, A / B matrices on L1 use Zz layout, demonstrating the special transfer method for float transposed input through `LoadDataWithTranspose`.

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: Data Layout Formats in Different Storage Units</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">GM</span></td>
    <td align="center"><span style="font-weight: bold;">L1</span></td>
    <td align="center"><span style="font-weight: bold;">L0A</span></td>
    <td align="center"><span style="font-weight: bold;">L0B</span></td>
    <td align="center"><span style="font-weight: bold;">L0C</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">Data Layout Format</span></td>
    <td align="center">ND</td>
    <td align="center">Nz</td>
    <td align="center">Zz</td>
    <td align="center">Zn</td>
    <td align="center">Zz</td>
  </tr>
</table>

14 scenarios are derived from combinations of input data type, `isAtranspose`, `isBtranspose`, L1 layout format, and L1 -> L0 transfer method. [Table 2](#table2) first describes the callable interfaces for the L1 -> L0 phase based on input data type and transpose requirements.

<a name="table2"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 2: L1 -> L0, Callable <code>LoadData</code> Instructions in Different Scenarios</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A non-transposed input [m, k]<br>L1 -> L0A no transpose needed</span></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code></td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">A transposed input [k, m]<br>L1 -> L0A transpose needed</span></td>
    <td align="center">Not supported</td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code>, <code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load3Dv2</code>, <br><code>LoadDataWithTranspose</code> (L1 data layout is Zz)</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B non-transposed input [k, n]<br>L1 -> L0B transpose needed</span></td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load2D</code>, <code>Load3Dv2</code>, <code>LoadDataWithTranspose</code></td>
    <td align="center"><code>Load3Dv2</code>, <br><code>LoadDataWithTranspose</code> (L1 data layout is Zz)</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B transposed input [n, k]<br>L1 -> L0B no transpose needed</span></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
    <td align="center"><code>Load2D</code></td>
  </tr>
</table>

`loadDataMode` is a general term used in this document to describe the L1 -> L0 transfer method, not an Ascend C interface parameter. In the code, A matrix and B matrix select the L1 -> L0 transfer method through template parameters `loadData2AMode` and `loadData2BMode` respectively:

- `0`: Use `Load2D`, that is `LoadData (LoadData2DParams)`.
- `1`: Use `LoadDataWithTranspose`.
- `2`: Use `Load3Dv2`, that is `LoadData (LoadData3DParamsV2)`.

### 2. Key Scenario Differences

The input shape, transpose flags, and L1 -> L0 transfer methods for each scenario are shown in [Table 3](#table3).

Note: When scenarioNum is 1 to 13, A and B matrices on L1 are all in Nz layout; when scenarioNum=14, A and B matrices on L1 are all in Zz layout.

<a name="table3"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 3: Meaning of Different scenarioNum Values</span></caption>
  <tr>
    <td><span style="font-weight: bold;">scenarioNum</span></td>
    <td><span style="font-weight: bold;">Input Data Type</span></td>
    <td><span style="font-weight: bold;">Output Data Type</span></td>
    <td><span style="font-weight: bold;">Input Shape</span></td>
    <td><span style="font-weight: bold;">isAtranspose</span></td>
    <td><span style="font-weight: bold;">isBtranspose</span></td>
    <td><span style="font-weight: bold;">A Matrix <code>LoadData</code> Instruction Type</span></td>
    <td><span style="font-weight: bold;">B Matrix <code>LoadData</code> Instruction Type</span></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">1</span></td>
    <td rowspan="2">int4_t</td>
    <td rowspan="2">int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">2</span></td>
    <td>A [40, 70]<br>B [70, 50]</td>
    <td>false</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">3</span></td>
    <td rowspan="3">int8_t</td>
    <td rowspan="3">int32_t</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">4</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">5</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">6</span></td>
    <td rowspan="5">half</td>
    <td rowspan="5">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">7</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">8</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">9</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">10</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load3Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">11</span></td>
    <td rowspan="3">float</td>
    <td rowspan="3">float</td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load2D</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">12</span></td>
    <td>A [40, 70]<br>B [50, 70]</td>
    <td>false</td>
    <td>true</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load2D</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">13</span></td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>Load3Dv2</code></td>
    <td><code>Load3Dv2</code></td>
  </tr>
  <tr>
    <td><span style="font-weight: bold;">14</span></td>
    <td rowspan="1">float</td>
    <td rowspan="1">float</td>
    <td>A [70, 40]<br>B [70, 50]</td>
    <td>true</td>
    <td>false</td>
    <td><code>LoadDataWithTranspose</code></td>
    <td><code>LoadDataWithTranspose</code></td>
  </tr>
</table>

This section describes key differences by transfer mode.

**`Load2D` non-transpose transfer: Scenarios 1 / 3 / 6 / 11**

- A matrix non-transposed input `[m, k]`, use `Load2D` to transfer to L0A, for loop along m direction, transferring `CeilDivision(k, fractalShape[1])` fractals in k direction at once.
- B matrix transposed input `[n, k]`, use `Load2D` to transfer to L0B, for loop along k direction, transferring `CeilDivision(n, fractalShape[0])` fractals in n direction at once.
- Both A / B matrices complete L1 -> L0 transfer and large fractal layout format changes by configuring `srcStride`, `dstGap` and other parameters.

**A-side `Load3Dv2` non-transpose transfer: Scenarios 2 / 4 / 7 / 12**

- A matrix non-transposed input `[m, k]`, use `Load3Dv2` to transfer from L1 to L0A, converting Nz layout on L1 to Zz layout needed by L0A.
- `Load3Dv2` configures N as 1, convolution kernel width and height as 1, padding as 0, stride as 1, dilation as 1. After image to column expansion, the data layout can be viewed as Nz fractal layout on L1.
- A matrix completes transfer by configuring `l1H`, `l1W`, `kExtension`, `mExtension` and other parameters.
- B matrix uses `LoadDataWithTranspose` or `Load2D` based on scenario: in scenario 2, B matrix non-transposed input `[k, n]` completes transposed transfer through `LoadDataWithTranspose`; in scenarios 4 / 7 / 12, B matrix transposed input `[n, k]` completes non-transposed transfer through `Load2D`.

**`LoadDataWithTranspose` transpose transfer: Scenarios 5 / 9 / 14**

- In scenarios 5 / 9, A / B matrices on L1 are all in Nz layout. A matrix transposed input `[k, m]`, B matrix non-transposed input `[k, n]`, both use `LoadDataWithTranspose` for transfer with transpose.
- In scenario 14, A / B matrices on L1 are all in Zz layout, both use `LoadDataWithTranspose` to complete L1 -> L0 transfer with transpose during the transfer process.

**`Load2D` transpose transfer: Scenario 8**

- Both A / B matrices use `Load2D` with `ifTranspose = true`.
- A matrix for loops along m direction, B matrix for loops along k direction; the number of fractals transferred per iteration is the same as in `Load2D` non-transpose scenarios.
- During L1 -> L0 transfer, `Load2D` completes small fractal transpose and large fractal layout format changes through `ifTranspose = true`.

**`Load3Dv2` transpose transfer: Scenarios 10 / 13**

- Both A / B matrices use `Load3Dv2`.
- A matrix completes L1 -> L0A transpose transfer by configuring `enTranspose`, `l1H`, `l1W`, `kExtension`, `mExtension` and other parameters.
- B matrix enables transpose by default when calling `Load3Dv2`, no need to configure `enTranspose`, other configurations are similar to A matrix transfer.

### 3. Parameter Description

The subsequent code and parameter descriptions repeatedly use fractal and alignment related variables. This section defines these concepts first.

- `fractalShape`: Small fractal shape. B8 / B16 / B32 input data types have shape `[16, 32 / sizeof(T)]`, where `T` represents the input data type; B4 input data type has shape `[16, 64]`. Fractal information for data types involved in this example is shown in [Table 4](#table4).
- `fractalSize`: Number of elements in 1 small fractal, see [Table 4](#table4).
- `fractalNum`: When L1 -> L0A / L0B requires transpose, the transfer interface transposes by square matrix. When multiple consecutive small fractals need to merge into a square matrix, `fractalNum` represents the number of small fractals in that square matrix, see [Table 4](#table4).

<a name="table4"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 4: Fractal Information for Different Data Types</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">fractalShape</span></td>
    <td align="center"><span style="font-weight: bold;">fractalSize</span></td>
    <td align="center"><span style="font-weight: bold;">fractalNum</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B4</span></td>
    <td align="center">[16, 64]</td>
    <td align="center">1024</td>
    <td align="center">4</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B8</span></td>
    <td align="center">[16, 32]</td>
    <td align="center">512</td>
    <td align="center">2</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B16</span></td>
    <td align="center">[16, 16]</td>
    <td align="center">256</td>
    <td align="center">1</td>
  </tr>
    <tr>
    <td align="center"><span style="font-weight: bold;">B32</span></td>
    <td align="center">[16, 8]</td>
    <td align="center">128</td>
    <td align="center">2</td>
  </tr>
</table>

- `CeilAlign`: Ceiling alignment operation. For example, when `m = 30`, `CeilAlign(30, 16) = 32`, meaning align m axis to 16, resulting in m axis length of 32.

  ```cpp
  __aicore__ inline uint16_t CeilAlign(uint16_t size, uint16_t alignValue) {
      return (size + alignValue - 1) / alignValue * alignValue;
  }
  ```

- `CeilDivision`: Ceiling division, generally used to compute the number of loops after ceiling alignment.
- `mAlignValue`: m axis aligns to `mAlignValue`. For example, `mAlignValue = 32` means m axis aligns to 32; similarly `nAlignValue`, `kAlignValue`.
- `mAlignL1` and `mAlignL0`: Aligned values of m axis when A matrix is on L1 and L0A respectively. Similarly `kAlignL1`, `kAlignL0`, `nAlignL1`, `nAlignL0`.
- `srcoffset` and `dstoffset`: On L1, the LocalTensor address offset when A / B matrix outer axis loops once; on L0A / L0B, the LocalTensor address offset when A / B matrix outer axis loops once.

### 4. Alignment Requirements

Note: For ease of understanding, this example defaults to using the m axis of A matrix and k axis of B matrix as outer axis loops, without considering the scenario where the longer axis between m and k is used as the outer axis.

Additionally, A and B matrices have different alignment requirements in row and col directions on L1 and L0. The alignment requirements for the 13 scenarios corresponding to scenarioNum values 1-13 in [Table 3](#table3) (L1 layout format is Nz) are summarized in [Table 5](#table5) and [Table 6](#table6):

<a name="table5"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 5: Alignment Requirements for A, B Matrices on L1 (L1 layout format is Nz)</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A non-transposed [m, k]</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A transposed [k, m]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B non-transposed [k, n]</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[0] * fractalNum</td>
    <td colspan="1" align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B transposed [n, k]</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
</table>

<a name="table6"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 6: Alignment Requirements for A, B Matrices on L0</span></caption>
  <tr>
    <td></td>
    <td align="center"><span style="font-weight: bold;">B4 (fractalNum=4)</span></td>
    <td align="center"><span style="font-weight: bold;">B8 (fractalNum=2)</span></td>
    <td align="center"><span style="font-weight: bold;">B16 (fractalNum=1)</span></td>
    <td align="center"><span style="font-weight: bold;">B32 (fractalNum=2)</span></td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A non-transposed [m, k], L1 -> L0A no transpose</span></td>
    <td colspan="4" align="center">mAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
  <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">A transposed [k, m], L1 -> L0A transpose needed</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
    <td>kAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td colspan="3" align="center">mAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center">mAlignValue = fractalShape[1]</td>
  </tr>
    <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B non-transposed [k, n], L1 -> L0B transpose needed</span></td>
    <td colspan="3" align="center">kAlignValue = fractalShape[1]</td>
      <td align="center">kAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="3" align="center">nAlignValue = fractalShape[0] * fractalNum</td>
    <td align="center">nAlignValue = fractalShape[1]</td>
  </tr>
 <tr>
    <td rowspan="2" align="center"><span style="font-weight: bold;">B transposed [n, k], L1 -> L0B no transpose</span></td>
    <td colspan="4" align="center">nAlignValue = fractalShape[0]</td>
  </tr>
  <tr>
    <td colspan="4" align="center">kAlignValue = fractalShape[1]</td>
  </tr>
</table>

For scenarioNum=14, the L1 layout format is Zz. The alignment requirements for A and B matrices on L1 and L0 in height and width directions are shown in [Table 7](#table7).

<a name="table7"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 7: scenarioNum=14, Alignment Requirements for A, B Matrices on L1 (Zz layout) and L0</span></caption>
  <tr>
    <td align="center"><span style="font-weight: bold;">Matrix Input</span></td>
    <td align="center"><span style="font-weight: bold;">L1 (Zz layout)</span></td>
    <td align="center"><span style="font-weight: bold;">L0</span></td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">A transposed [k, m]</span><br>float (fractalNum=2)</td>
    <td align="center">kAlignValue = fractalShape[0]<br>mAlignValue = fractalShape[1] * fractalNum</td>
    <td align="center">mAlignValue = fractalShape[0]<br>kAlignValue = fractalShape[1] * fractalNum</td>
  </tr>
  <tr>
    <td align="center"><span style="font-weight: bold;">B non-transposed [k, n]</span><br>float (fractalNum=2)</td>
    <td align="center">kAlignValue = fractalShape[0]<br>nAlignValue = fractalShape[1] * fractalNum</td>
    <td align="center">kAlignValue = fractalShape[1] * fractalNum<br>nAlignValue = fractalShape[0]</td>
  </tr>
</table>

### 5. GM to L1 (`DataCopy`)

This section describes how to call the `DataCopy` interface and configure `Nd2NzParams` to transfer A / B matrices from ND layout on GM to L1. First determine the L1 target layout based on the scenario:

- **Scenarios 1-13**: A / B matrices transferred to L1 are all in Nz layout, standard ND -> Nz transfer.
- **Scenario 14**: A / B matrices transferred to L1 are all in Zz layout, used for subsequent float transpose transfer through `LoadDataWithTranspose`.

Key parameters to focus on when configuring `Nd2NzParams`:

- `nValue`, `dValue`: Logical shape of source ND matrix.
- `dstNzC0Stride`: Length of Nz matrix on L1 after row direction alignment, unit is 32B.
- `ndNum`, `srcNdMatrixStride`, `dstNzMatrixStride`: Only needed when constructing Zz layout by splitting multiple ND matrices along height axis.

For detailed parameter configurations for A matrix GM -> L1 (both [m, k] and [k, m] inputs, Nz and Zz targets) and B matrix GM -> L1 (both [k, n] and [n, k] inputs, Nz and Zz targets), please refer to the Chinese README for the complete code examples and figures.

### 6. L1 to L0 (`LoadData`)

This section describes how to transfer A / B matrices from L1 to L0A / L0B. First determine the current scenario from [Table 3](#table3), then enter the corresponding subsection:

- **Non-transpose transfer**: Only large fractal layout format changes occur. A matrix transferred to L0A becomes Zz, B matrix transferred to L0B becomes Zn.
- **Transpose transfer**: Both small fractal transpose and large fractal layout format changes occur.
- **L1 is Nz layout**: Can select `Load2D`, `LoadDataWithTranspose`, or `Load3Dv2` based on data type.
- **L1 is Zz layout**: Scenario 14 uses `LoadDataWithTranspose` for float transpose transfer.

Focus on `repeatTimes`, `srcStride`, `dstGap`, `dstFracGap`, `ifTranspose`, and `enTranspose` parameters in code blocks, as they determine the number of fractals per transfer, adjacent fractal intervals, and whether to transpose.

For detailed parameter configurations and code examples for each sub-scenario (A matrix L1 -> L0A non-transpose, A matrix L1 -> L0A transpose with int8_t/half/float, B matrix L1 -> L0B non-transpose, B matrix L1 -> L0B transpose), please refer to the Chinese README for the complete code examples and figures.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables
  Configure environment variables according to the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
  ./demo
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin
  ```
  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;   # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: Atlas A2/A3 series products |
  | `SCENARIO_NUM` | `1`-`14` | Scenario number: different data types and transpose combinations |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
