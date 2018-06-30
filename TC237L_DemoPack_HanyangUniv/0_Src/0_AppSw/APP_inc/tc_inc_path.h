/*====================================================================
* Project:  Board Support Package (BSP)
* Function: macros for handling different AURIX devices and boards
*
* Copyright HighTec EDV-Systeme GmbH 1982-2015
*====================================================================*/

#ifndef __TC_INC_PATH_H__
#define __TC_INC_PATH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(__TC29XX__)
# if defined(TRIBOARD_TC29XX)
#  define TCPATH	tc29xx
# elif defined(TRIBOARD_TC29XA)
#  define TCPATH	tc29xa
# elif defined(TRIBOARD_TC29XB) || defined(APPKIT_TC297TFT)
#  define TCPATH	tc29xb
# else
#  error "No TC29x board defined"
# endif /* TRIBOARD_TC29XX */
#elif defined(__TC26XX__)
# if defined(TRIBOARD_TC26XX)
#  define TCPATH	tc26xx
# elif defined(TRIBOARD_TC26XA) || defined(APPKIT_TC265A)
#  define TCPATH	tc26xa
# elif defined(TRIBOARD_TC26XB) || defined(APPKIT_TC265B) || defined(APPKIT_TC267TFT)
#  define TCPATH	tc26xb
# else
#  error "No TC26x board defined"
# endif /* TRIBOARD_TC26XX */
#elif defined(__TC27XX__)
# if defined(TRIBOARD_TC275B)
#  define TCPATH	tc27xb
# elif defined(APPKIT_TC275TU_C) || defined(TRIBOARD_TC275C) || defined(APPKIT_TC277TFT)
#  define TCPATH	tc27xc
# elif defined(APPKIT_TC275TU) || defined(APPKIT_TC275TE) || defined(TRIBOARD_TC275A)
#  define TCPATH	tc27xa
# else
#  error "No TC27x board defined"
# endif /* TRIBOARD_TC275B */
#elif defined(__TC23XX__)
# if defined(APPKIT_TC23XX) || defined(APPKIT_TC234TFT) || defined(APPKIT_TC237TFT) || defined(TRIBOARD_TC233A) || defined(TRIBOARD_TC23XX)
#  define TCPATH	tc23xa
# else
#  error "No TC23x board defined"
# endif /* APPKIT_TC23XX */
#elif defined(__TC22XX__)
# if defined(APPKIT_TC22XX) || defined(APPKIT_TC224TFT) || defined(TRIBOARD_TC223A) || defined(TRIBOARD_TC22XX)
#  define TCPATH	tc22xa
# else
#  error "No TC22x board defined"
# endif /* APPKIT_TC22XX */
#else
#  error "Unsupported AURIX device"
#endif /* __TC29XX__ */

#define TC_STR(s)		# s
#define TC_INCLUDE(f)	TC_STR(f)

/* check for application kits */
#if defined(APPKIT_TC275TU) || defined(APPKIT_TC275TE) || defined(APPKIT_TC275TU_C) || defined(APPKIT_TC265B) || defined(APPKIT_TC265A)
#define APPKIT_TC2X5	1
#else
#define APPKIT_TC2X5	0
#endif /* APPKIT_TC2X5 */
#if defined(APPKIT_TC297TFT) || defined(APPKIT_TC277TFT) || defined(APPKIT_TC267TFT) || defined(APPKIT_TC237TFT)
#define APPKIT_TC2X7	1
#else
#define APPKIT_TC2X7	0
#endif /* APPKIT_TC2X7 */
#if defined(APPKIT_TC23XX) || defined(APPKIT_TC234TFT) || defined(APPKIT_TC22XX) || defined(APPKIT_TC224TFT)
#define APPKIT_TC2X4	1
#else
#define APPKIT_TC2X4	0
#endif /* APPKIT_TC2X7 */
#if (APPKIT_TC2X7 > 0) || (APPKIT_TC2X5 > 0) || (APPKIT_TC2X4 > 0)
#define RUN_ON_APPKIT	1
#else
#define RUN_ON_APPKIT	0
#endif /* APPKIT_TC2XX */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TC_INC_PATH_H__ */
