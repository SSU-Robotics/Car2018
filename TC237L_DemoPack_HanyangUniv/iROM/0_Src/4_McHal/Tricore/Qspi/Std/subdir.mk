################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../0_Src/4_McHal/Tricore/Qspi/Std/IfxQspi.c 

OBJS += \
./0_Src/4_McHal/Tricore/Qspi/Std/IfxQspi.o 

C_DEPS += \
./0_Src/4_McHal/Tricore/Qspi/Std/IfxQspi.d 


# Each subdirectory must supply rules for building sources it contributes
0_Src/4_McHal/Tricore/Qspi/Std/%.o: ../0_Src/4_McHal/Tricore/Qspi/Std/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TriCore C Compiler'
	"$(TRICORE_TOOLS)/bin/tricore-gcc" -c -IC:/Users/kimwayne/HanyangUniv_Project/TC237L_DemoPack_HanyangUniv/h -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Qspi\SpiMaster" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Multican" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Multican\Can" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Multican\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\_Impl" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\_Lib\DataHandling" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\_Lib\InternalMux" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\_PinMap" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Asclin\Asc" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Asclin\Lin" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Asclin\Spi" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Asclin\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Cpu\CStart" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Cpu\Irq" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Cpu\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Cpu\Trap" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Dma\Dma" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Dma\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Gtm\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Gtm\Tom" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Gtm\Trig" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Port\Io" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Port\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Scu\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Src\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Stm\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Vadc\Adc" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\Vadc\Std" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore\_Reg" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\Config\Common" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\Tricore\Compilers" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\0_AppSw\APP_inc" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\_Utilities" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\Config" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\If" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\StdIf" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\SysSe" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\1_SrvSw\Tricore" -I"C:\Users\tlarb\workspace\TC237L_DemoPack_HanyangUniv\0_Src\4_McHal\Tricore" -fno-common -Os -g3 -W -Wall -Wextra -Wdiv-by-zero -Warray-bounds -Wcast-align -Wignored-qualifiers -Wformat -Wformat-security -DAPPKIT_TC237TFT -fshort-double -mcpu=tc23xx -mversion-info -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


