# Reference design for Xilinx DMA FIFO

## Hardware

In this design, the Xilinx DMA is configured without Scatter Gather Engine and non-Micro DMA mode. A FIFO is configured in streaming mode as a buffer between the source and destination address of DMA. The data width is 32 bits while HP ports of PS are adopted as streaming input/output interfaces. 

<img src="https://github.com/wincle626/ZCU106_DMA_REF_DESIGN/blob/main/figures/fifo_blockdiagram.png" alt="fifoblockdiagram"
	title="FIFO block diagram" width="720" height="200" />

## Software

## Experiments
