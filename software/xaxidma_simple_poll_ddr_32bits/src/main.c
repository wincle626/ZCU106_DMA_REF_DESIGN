/*
 * xaxidma_register_wr.c
 *
 *  Created on: 2021Äê3ÔÂ7ÈÕ
 *      Author: vv_fa
 */

#include <stdio.h>
#include <getopt.h>
#include <sys/mman.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "xaxidma.h"
#include "xparameters.h"
#include "xdebug.h"


//#define TX_BUFFER_BASE		(0xFFFC0000)
//#define RX_BUFFER_BASE		(0xFFFD0000)
#define TX_BUFFER_BASE		(0x01100000)
#define RX_BUFFER_BASE		(0x01300000)

float *TxBufferPtr;
float *RxBufferPtr;
void *TxBufferPtr_vaddr;
void *RxBufferPtr_vaddr;
void *TxBufferPtr_paddr;
void *RxBufferPtr_paddr;

#define MAX_PKT_LEN		16
#define BYTES sizeof(float)

typedef struct{
	int real;
	int imag;
}Complex;

int XAxiDma_Check(XAxiDma * InstancePtr);
int floatstream(float value);
int complexstream(Complex values);

int main(int argc, char** argv){

	if(argc<2){
		printf("please input a starting value!\n");
		return 1;
	}

	float value = atof(argv[1]);

	floatstream(value);

	return XST_SUCCESS;
}

int floatstream(float value){

	int Status;

	/* Initialize the addresses
	 */
	XAxiDma_Config *CfgPtr;
	XAxiDma AxiDma;
	int fd = open("/dev/mem", O_RDWR);
	TxBufferPtr_paddr = (float *) TX_BUFFER_BASE;
	RxBufferPtr_paddr = (float *) RX_BUFFER_BASE;
	TxBufferPtr_vaddr   = mmap(NULL, MAX_PKT_LEN*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd,
			TX_BUFFER_BASE);
	RxBufferPtr_vaddr = mmap(NULL, MAX_PKT_LEN*sizeof(float), PROT_READ|PROT_WRITE, MAP_SHARED, fd,
			RX_BUFFER_BASE);
	TxBufferPtr = (float *) TxBufferPtr_vaddr;
	RxBufferPtr = (float *) RxBufferPtr_vaddr;
	printf("TxBufferPtr_paddr-0x%X, RxBufferPtr_paddr-0x%X\n "
			"TxBufferPtr_vaddr-0x%X, RxBufferPtr_vaddr-0x%X\n"
			"TxBufferPtr-0x%X, RxBufferPtr-0x%X\n",
			(unsigned int)TxBufferPtr_paddr, (unsigned int)RxBufferPtr_paddr,
			(unsigned int)TxBufferPtr_vaddr, (unsigned int)RxBufferPtr_vaddr,
			(unsigned int)TxBufferPtr, (unsigned int)RxBufferPtr);

	/* Initialize the XAxiDma device.
	 */
	CfgPtr = XAxiDma_LookupConfig(0);
	if (!CfgPtr) {
		printf("No DMA config found for %d\n\n", 0);
		return XST_FAILURE;
	}else{
		printf("DMA config found for %d\n\n", 0);
	}
	printf("Physical DMA BaseAddr 0x%lX\n\n", CfgPtr->BaseAddr);
	CfgPtr->BaseAddr   = (UINTPTR)mmap(NULL, MAX_PKT_LEN*sizeof(float),
						PROT_READ|PROT_WRITE, MAP_SHARED, fd,
						CfgPtr->BaseAddr);
	printf("Virtual DMA BaseAddr 0x%lX\n\n", CfgPtr->BaseAddr);
	Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
	if (Status != XST_SUCCESS) {
		printf("Initialization failed %d\n\n", Status);
		return XST_FAILURE;
	}else{
		printf("Initialization succeeded %d\n\n", Status);
	}
	XAxiDma_WriteReg(AxiDma.TxBdRing.ChanBase,
			 XAXIDMA_SRCADDR_OFFSET,
			 LOWER_32_BITS((UINTPTR)TxBufferPtr_paddr));
	printf("XAxiDma_WriteReg(AxiDma.TxBdRing.ChanBase,"
			"XAXIDMA_SRCADDR_OFFSET, "
			"LOWER_32_BITS((UINTPTR)TxBufferPtr_paddr)) 0x%X\n\n",
			LOWER_32_BITS((UINTPTR)TxBufferPtr_paddr));
	if (AxiDma.AddrWidth > 32){
		printf("XAxiDma_WriteReg(AxiDma.TxBdRing.ChanBase,"
				"XAXIDMA_SRCADDR_MSB_OFFSET,"
				"UPPER_32_BITS((UINTPTR)TxBufferPtr_paddr) 0x%X\n\n",
				UPPER_32_BITS((UINTPTR)TxBufferPtr_paddr));
		XAxiDma_WriteReg(AxiDma.TxBdRing.ChanBase,
				 XAXIDMA_SRCADDR_MSB_OFFSET,
				 UPPER_32_BITS((UINTPTR)TxBufferPtr_paddr));
	}
	XAxiDma_WriteReg(AxiDma.TxBdRing.ChanBase,
				XAXIDMA_BUFFLEN_OFFSET, MAX_PKT_LEN*BYTES);
	printf("XAxiDma_WriteReg(AxiDma.TxBdRing.ChanBase, "
			"XAXIDMA_BUFFLEN_OFFSET, MAX_PKT_LEN) 0x%X\n\n",
			MAX_PKT_LEN*BYTES);
	XAxiDma_WriteReg(AxiDma.RxBdRing[0].ChanBase,
			 XAXIDMA_DESTADDR_OFFSET,
			 LOWER_32_BITS((UINTPTR)RxBufferPtr_paddr));
	printf("XAxiDma_WriteReg(AxiDma.RxBdRing[0].ChanBase,"
			"XAXIDMA_DESTADDR_OFFSET, "
			"LOWER_32_BITS((UINTPTR)RxBufferPtr_paddr) 0x%X\n\n",
			LOWER_32_BITS((UINTPTR)RxBufferPtr_paddr));
	if (AxiDma.AddrWidth > 32){
		XAxiDma_WriteReg(AxiDma.RxBdRing[0].ChanBase,
				 XAXIDMA_DESTADDR_MSB_OFFSET,
				 UPPER_32_BITS((UINTPTR)RxBufferPtr_paddr));
		printf("XAxiDma_WriteReg(AxiDma.RxBdRing[0].ChanBase, "
				"XAXIDMA_DESTADDR_MSB_OFFSET, "
				"UPPER_32_BITS((UINTPTR)RxBufferPtr_paddr)) 0x%X\n\n",
				UPPER_32_BITS((UINTPTR)RxBufferPtr_paddr));
	}
	XAxiDma_WriteReg(AxiDma.RxBdRing[0].ChanBase,
				XAXIDMA_BUFFLEN_OFFSET, MAX_PKT_LEN*BYTES);
	printf("XAxiDma_WriteReg(AxiDma.RxBdRing[0].ChanBase, "
			"XAXIDMA_BUFFLEN_OFFSET, MAX_PKT_LEN) 0x%X\n\n",
			MAX_PKT_LEN*BYTES);
	if(!(XAxiDma_Check(&AxiDma)==XST_SUCCESS)){
		printf("Check DMA found issue\n\n");
		return XST_FAILURE;
	}else{
		printf("Check DMA found no issue\n\n");
	}

	/* Initialize the input data.
	 */
	printf("Before data streaming: \n");
	int Index;
	for(Index = 0; Index < MAX_PKT_LEN; Index ++) {
		TxBufferPtr[Index] = (float) value + (rand()%100)/10;
		printf("TxBufferPtr[%d] = (float) %f\n", Index, TxBufferPtr[Index]);
	}
	for(Index = 0; Index < MAX_PKT_LEN; Index ++) {
		printf("RxBufferPtr[%d] = (float) %f\n", Index, RxBufferPtr[Index]);
	}
	printf("\n");
	printf("\n");


	/* Do the DMA simple transfer.
	 */
	Status = XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) RxBufferPtr_paddr,
				MAX_PKT_LEN*BYTES, XAXIDMA_DEVICE_TO_DMA);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = XAxiDma_SimpleTransfer(&AxiDma,(UINTPTR) TxBufferPtr_paddr,
				MAX_PKT_LEN*BYTES, XAXIDMA_DMA_TO_DEVICE);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	while ((XAxiDma_Busy(&AxiDma,XAXIDMA_DEVICE_TO_DMA)) ||
		(XAxiDma_Busy(&AxiDma,XAXIDMA_DMA_TO_DEVICE))) {
			/* Wait */
	}

//	usleep(1000*BYTES000);

	/* Check the output data.
	 */
	printf("After data streaming: \n");
	for(Index = 0; Index < MAX_PKT_LEN; Index ++) {
		printf("TxBufferPtr[%d] = (float) %f\n", Index, (float) TxBufferPtr[Index]);
	}
	for(Index = 0; Index < MAX_PKT_LEN; Index ++) {
		printf("RxBufferPtr[%d] = (float) %f\n", Index, (float) RxBufferPtr[Index]);
	}
	printf("\n");

	return Status;

}

int complexstream(Complex values){

	int Status;

	return Status;

}

int XAxiDma_Check(XAxiDma * InstancePtr)
{

//	int Status = 0;

	if (!InstancePtr->Initialized) {

		printf("Pause: Driver not initialized"
					" %d\n\n",InstancePtr->Initialized);

		return XST_NOT_SGDMA;
	}

	if(XAxiDma_HasSg(InstancePtr)){
		printf("Device configured as SG mode \n\n");
		return XST_FAILURE;
	}else{
		printf("Device configured as no SG mode \n\n");
	}

	if (InstancePtr->HasMm2S) {
		XAxiDma_BdRing *TxRingPtr;
		TxRingPtr = XAxiDma_GetTxRing(InstancePtr);

		printf("Now check TxRingPtr\n\n");
		XAxiDma_BdRingDumpRegs(TxRingPtr);

		if(!(XAxiDma_ReadReg(InstancePtr->TxBdRing.ChanBase,
				XAXIDMA_SRCADDR_OFFSET)==TX_BUFFER_BASE)){
			printf("XAxiDma_ReadReg(AxiDma.TxBdRing.ChanBase,"
					"XAXIDMA_SRCADDR_OFFSET)  0x%X\n\n",
					XAxiDma_ReadReg(InstancePtr->TxBdRing.ChanBase,
							XAXIDMA_SRCADDR_OFFSET));
			printf("XAxiDma_ReadReg(AxiDma.TxBdRing.ChanBase,"
					"XAXIDMA_SRCADDR_MSB_OFFSET) 0x%X\n\n",
					XAxiDma_ReadReg(InstancePtr->TxBdRing.ChanBase,
							XAXIDMA_SRCADDR_MSB_OFFSET));
			return XST_FAILURE;
		}

		if(!(XAxiDma_ReadReg(InstancePtr->TxBdRing.ChanBase,
				XAXIDMA_BUFFLEN_OFFSET)==MAX_PKT_LEN*BYTES)){
			printf("XAxiDma_ReadReg(AxiDma.TxBdRing.ChanBase, "
					"XAXIDMA_BUFFLEN_OFFSET) 0x%X\n\n",
					XAxiDma_ReadReg(InstancePtr->TxBdRing.ChanBase,
							XAXIDMA_BUFFLEN_OFFSET));
			return XST_FAILURE;
		}
	}

	if (InstancePtr->HasS2Mm) {
		int RingIndex = 0;
		for (RingIndex = 0; RingIndex < InstancePtr->RxNumChannels;
				RingIndex++) {
			XAxiDma_BdRing *RxRingPtr;
			RxRingPtr = XAxiDma_GetRxIndexRing(InstancePtr, RingIndex);

			printf("Now check RxRingPtr\n\n");
			XAxiDma_BdRingDumpRegs(RxRingPtr);

			if(!(XAxiDma_ReadReg(InstancePtr->RxBdRing[RingIndex].ChanBase,
					 XAXIDMA_DESTADDR_OFFSET)==RX_BUFFER_BASE)){
				printf("XAxiDma_ReadReg(AxiDma.RxBdRing[RingIndex].ChanBase,"
						"XAXIDMA_DESTADDR_OFFSET)  0x%X\n\n",
						XAxiDma_ReadReg(InstancePtr->RxBdRing[RingIndex].ChanBase,
								XAXIDMA_DESTADDR_OFFSET));
				printf("XAxiDma_ReadReg(AxiDma.RxBdRing[RingIndex].ChanBase,"
						"XAXIDMA_DESTADDR_MSB_OFFSET) 0x%X\n\n",
						XAxiDma_ReadReg(InstancePtr->RxBdRing[RingIndex].ChanBase,
								XAXIDMA_DESTADDR_MSB_OFFSET));
				printf("XAxiDma_ReadReg(AxiDma.RxBdRing[0].ChanBase, "
						"XAXIDMA_BUFFLEN_OFFSET) 0x%X\n\n",
						XAxiDma_ReadReg(InstancePtr->RxBdRing[0].ChanBase,
								XAXIDMA_BUFFLEN_OFFSET));
				return XST_FAILURE;
			}

			if(!(XAxiDma_ReadReg(InstancePtr->RxBdRing[0].ChanBase,
								XAXIDMA_BUFFLEN_OFFSET)==MAX_PKT_LEN*BYTES)){
				printf("XAxiDma_ReadReg(AxiDma.RxBdRing[0].ChanBase, "
						"XAXIDMA_BUFFLEN_OFFSET) 0x%X\n\n",
						XAxiDma_ReadReg(InstancePtr->RxBdRing[0].ChanBase,
								XAXIDMA_BUFFLEN_OFFSET));
			}
		}
	}

	return XST_SUCCESS;

}
