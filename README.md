# Reference design for Xilinx DMA FIFO

## Hardware

In this design, the Xilinx DMA is configured without Scatter Gather Engine and non-Micro DMA mode. A FIFO is configured in streaming mode as a buffer between the source and destination address of DMA. The package mode is enabled for the FIFO which means the data streamed either the FIFO is full or AXI bus tlast signal is asserted. The data width is 32 bits while HP ports of PS are adopted as streaming input/output interfaces. 

<img src="https://github.com/wincle626/ZCU106_DMA_REF_DESIGN/blob/main/figures/fifo_blockdiagram.png" alt="fifoblockdiagram"
	title="FIFO block diagram" width="720" height="200" />

## Software

The source and destination address are mapped as virtual memory to be accessed under Linux. By streaming specific data from the source address, the received data at the destination address is checked by comparing the pre-defined data array. 

## Experiments

### 1. Resource utilization

All the designs target 100 MHz. 

| FFT  |      16      |      64      |      256     |     1024     |
| ---- | ------------ | ------------ | ------------ | ------------ |
| LUT  | 3724 (1.62%) | 3765 (1.62%) | 3717 (1.61%) | 3720 (1.61%) |
| FF   | 5170 (1.12%) | 5178 (1.12%) | 5112 (1.11%) | 5120 (1.11%) |
| BRAM |    5 (1.60%) |    5 (1.60%) |    6 (2.08%) |  6.5 (2.08%) |

### 2. Power consumption

1. Depth 16 FIFO

| VCCINT (volt) |       Power (mW)       | VCCBRAM (volt) |       Power (mW)       |
| ------------- | ---------------------- | -------------- | ---------------------- |
|      0.85     |         453.81         |      0.90      |          57.52         |
|      0.80     |         403.47         |      0.85      |          48.36         |
|      0.75     |         363.55         |      0.80      |          40.08         |
|      0.70     |         326.38         |      0.75      |          32.65         |
|      0.65     |         290.67         |      0.70      |          25.73         |
|      0.60     |         257.62         |      0.65      |          19.54         |
|      0.55     |         225.77         |      0.60      |          13.71         |
|      0.52     |         207.11         |      0.59      |          12.48         |
|      0.51     |          N/A           |      0.58      |           N/A          |

2. Depth 1024 FIFO

| VCCINT (volt) |       Power (mW)       | VCCBRAM (volt) |       Power (mW)       |
| ------------- | ---------------------- | -------------- | ---------------------- |
|      0.85     |         453.81         |      0.90      |          57.52         |
|      0.80     |         403.47         |      0.85      |          48.36         |
|      0.75     |         363.55         |      0.80      |          40.08         |
|      0.70     |         326.38         |      0.75      |          32.65         |
|      0.65     |         290.67         |      0.70      |          25.73         |
|      0.60     |         257.62         |      0.65      |          19.54         |
|      0.55     |         225.77         |      0.60      |          13.71         |
|      0.52     |         207.11         |      0.59      |          12.48         |
|      0.51     |          N/A           |      0.58      |           N/A          |

## Undervolt Error Detection

So far, by tuning both VCCINT and VCCBRAM to the lowest voltage before crash, there is no error captured by comparing the received data at destination address to the pre-defined input data. 
