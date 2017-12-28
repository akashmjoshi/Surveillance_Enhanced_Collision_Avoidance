################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
evmdm6437bsl.out: ../evmdm6437bsl.lib $(GEN_CMDS)
	@echo 'Building file: $<'
	@echo 'Invoking: Linker'
	"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400+ -g --define="_DEBUG" --quiet --diag_warning=225 --abi=coffabi -z -m"video_loopback.map" --warn_sections -i"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/lib" -i"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/include" -i"C:/Users/DSP Lab 9/Desktop/video/video_loopback" -i"C:/Users/DSP Lab 6/Downloads/Video_Template_6437/Video_Template_1_6437" -i"/lib" -i"C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/csl/lib" -i"C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/dsk6416/lib" -i"C:/Program Files (x86)/Texas Instruments/ccsv4/evmdm6437_v2/lib" --reread_libs --rom_model -o "$@" "$<" "../linker.cmd" "../evmdm6437bsl.lib"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400+ -g --define="_DEBUG" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="C:/Program Files (x86)/Texas Instruments/xdais_7_10_00_06/packages/ti/xdais" --include_path="/include" --include_path="C:/Users/DSP Lab 6/Downloads/Video_Template_6437/Video_Template_1_6437/../../include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/evmdm6437_v2/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/dsk6416/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/csl/include" --quiet --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

tvp5146.obj: ../tvp5146.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400+ -g --define="_DEBUG" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="C:/Program Files (x86)/Texas Instruments/xdais_7_10_00_06/packages/ti/xdais" --include_path="/include" --include_path="C:/Users/DSP Lab 6/Downloads/Video_Template_6437/Video_Template_1_6437/../../include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/evmdm6437_v2/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/dsk6416/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/csl/include" --quiet --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="tvp5146.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '

video_loopback_test.obj: ../video_loopback_test.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/bin/cl6x" -mv6400+ -g --define="_DEBUG" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/tools/compiler/c6000/include" --include_path="C:/Program Files (x86)/Texas Instruments/xdais_7_10_00_06/packages/ti/xdais" --include_path="/include" --include_path="C:/Users/DSP Lab 6/Downloads/Video_Template_6437/Video_Template_1_6437/../../include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/evmdm6437_v2/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/dsk6416/include" --include_path="C:/Program Files (x86)/Texas Instruments/ccsv4/C6000/csl/include" --quiet --diag_warning=225 --abi=coffabi --preproc_with_compile --preproc_dependency="video_loopback_test.pp" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


