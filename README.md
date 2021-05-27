# Reference design for Xilinx DMA FIFO

## Hardware

In this design, the Xilinx DMA is configured without Scatter Gather Engine and non-Micro DMA mode. A FIFO is configured in streaming mode as a buffer between the source and destination address of DMA. The package mode is enabled for the FIFO which means the data streamed either the FIFO is full or AXI bus tlast signal is asserted. The data width is 32 bits while HP ports of PS are adopted as streaming input/output interfaces. 

<img src="https://github.com/wincle626/ZCU106_DMA_REF_DESIGN/blob/main/figures/fifo_blockdiagram.png" alt="fifoblockdiagram"
	title="FIFO block diagram" width="720" height="200" />

## Software

The source and destination address are mapped as virtual memory to be accessed under Linux. By streaming specific data from the source address, the received data at the destination address is checked by comparing the pre-defined data array. 

## Experiments
