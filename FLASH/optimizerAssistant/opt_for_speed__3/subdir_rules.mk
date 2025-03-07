################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O2 --opt_for_speed=3 --fp_mode=relaxed --include_path="/Users/peter/workspace_v12/NMS1_20240826-5" --include_path="/Users/peter/workspace_v12/NMS1_20240826-5/Lib/F2837xS_common" --include_path="/Users/peter/workspace_v12/NMS1_20240826-5/Lib/F2837xS_common/driverlib" --include_path="/Applications/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_common/driverlib" --include_path="/Applications/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_common/include" --include_path="/Applications/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_headers/include" --include_path="/Applications/ti/controlSUITE/libs/app_libs/motor_control/math_blocks/v4.3" --include_path="C:/ti/controlSUITE/libs/math/IQmath/v160/include" --include_path="/Applications/ti/controlSUITE/libs/math/IQmath/v160/include" --include_path="C:/ti/controlSUITE/libs/app_libs/motor_control/math_blocks/v4.3" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_common/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_headers/include" --include_path="/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O2 --opt_for_speed=3 --fp_mode=relaxed --include_path="/Users/peter/workspace_v12/NMS1_20240826-5" --include_path="/Users/peter/workspace_v12/NMS1_20240826-5/Lib/F2837xS_common" --include_path="/Users/peter/workspace_v12/NMS1_20240826-5/Lib/F2837xS_common/driverlib" --include_path="/Applications/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_common/driverlib" --include_path="/Applications/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_common/include" --include_path="/Applications/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_headers/include" --include_path="/Applications/ti/controlSUITE/libs/app_libs/motor_control/math_blocks/v4.3" --include_path="C:/ti/controlSUITE/libs/math/IQmath/v160/include" --include_path="/Applications/ti/controlSUITE/libs/math/IQmath/v160/include" --include_path="C:/ti/controlSUITE/libs/app_libs/motor_control/math_blocks/v4.3" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_common/include" --include_path="C:/ti/controlSUITE/device_support/F2837xS/v210/F2837xS_headers/include" --include_path="/Applications/ti/ccs1260/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


