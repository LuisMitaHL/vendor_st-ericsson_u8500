
#ifndef _MMDSP_H_
#define _MMDSP_H_

/* define for INTEG */
#define INTEG_BASE_ADDR                                   (0x5E400U)
#define INTEG_IPEN   (INTEG_BASE_ADDR + (0x50))
#define INTEG_ITOP_2 (INTEG_BASE_ADDR + (0x60))

#define HV_HW_REL            0x0412U

#define ITC_BASE_ADDR                                   (0x5BC00U)
/* -------------------------------------------------------------------- */
/*  Interrupt Controller sub-addresses */
/* -------------------------------------------------------------------- */
/* noexp register name				base + offset		size	def value	acces	type	xiordy */
#define ITC_CFG_TIM_L            (ITC_BASE_ADDR + (0x0000U))
#define ITC_CFG_TIM_H            (ITC_BASE_ADDR + (0x0002U))
#define ITC_CFG_TIC_L            (ITC_BASE_ADDR + (0x0004U))
#define ITC_CFG_TIC_H            (ITC_BASE_ADDR + (0x0006U))
#define ITC_CFG_IIS_L            (ITC_BASE_ADDR + (0x0020U))
#define ITC_CFG_IIS_H            (ITC_BASE_ADDR + (0x0022U))
#define ITC_CFG_ISR_L            (ITC_BASE_ADDR + (0x0024U))
#define ITC_CFG_ISR_H            (ITC_BASE_ADDR + (0x0026U))
#define ITC_CFG_IMR_L            (ITC_BASE_ADDR + (0x0028U))
#define ITC_CFG_IMR_H            (ITC_BASE_ADDR + (0x002AU))
/* noexp register name				base + offset		size	def value	acces	type	xiordy */
#define ITC_GRB_CTL_L            (ITC_BASE_ADDR + (0x0100U))
#define ITC_GRB_CTL_H            (ITC_BASE_ADDR + (0x0102U))
#define ITC_GRB_SEM_L            (ITC_BASE_ADDR + (0x0104U))
#define ITC_GRB_SEM_H            (ITC_BASE_ADDR + (0x0106U))
#define ITC_GRB_STA_L            (ITC_BASE_ADDR + (0x0120U))
#define ITC_GRB_STA_H            (ITC_BASE_ADDR + (0x0122U))
#define ITC_GRB_ISR_L            (ITC_BASE_ADDR + (0x0124U))
#define ITC_GRB_ISR_H            (ITC_BASE_ADDR + (0x0126U))
#define ITC_GRB_IMR_L            (ITC_BASE_ADDR + (0x0128U))
#define ITC_GRB_IMR_H            (ITC_BASE_ADDR + (0x012AU))
/* noexp register name				base + offset		size	def value	acces	type	xiordy */
#define ITC_ITC_CMP0_L           (ITC_BASE_ADDR + (0x0200U))
#define ITC_ITC_CMP0_H           (ITC_BASE_ADDR + (0x0202U))
#define ITC_ITC_CMP1_L           (ITC_BASE_ADDR + (0x0204U))
#define ITC_ITC_CMP1_H           (ITC_BASE_ADDR + (0x0206U))
#define ITC_ITC_CMP2_L           (ITC_BASE_ADDR + (0x0208U))
#define ITC_ITC_CMP2_H           (ITC_BASE_ADDR + (0x020AU))
#define ITC_ITC_CMP3_L           (ITC_BASE_ADDR + (0x020CU))
#define ITC_ITC_CMP3_H           (ITC_BASE_ADDR + (0x020EU))
#define ITC_CFG_ISS_L            (ITC_BASE_ADDR + (0x0220U))
#define ITC_GRB_STS_L            (ITC_BASE_ADDR + (0x0320U))
#define ITC_GRB_ISS_L            (ITC_BASE_ADDR + (0x0324U))

#endif /* _MMDSP_H_ */
