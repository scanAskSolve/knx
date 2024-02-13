#pragma once

#define DPT_Switch Dpt(1, 1, 0)
#define DPT_Bool Dpt(1, 2, 0)
#define DPT_Enable Dpt(1, 3, 0)
#define DPT_Ramp Dpt(1, 4, 0)
#define DPT_Alarm Dpt(1, 5, 0)
#define DPT_BinaryValue Dpt(1, 6, 0)
#define DPT_Step Dpt(1, 7, 0)
#define DPT_UpDown Dpt(1, 8, 0)
#define DPT_OpenClose Dpt(1, 9, 0)
#define DPT_Start Dpt(1, 10, 0)
#define DPT_State Dpt(1, 11, 0)
#define DPT_Invert Dpt(1, 12, 0)
#define DPT_DimSendStyle Dpt(1, 13, 0)
#define DPT_InputSource Dpt(1, 14, 0)
#define DPT_Reset Dpt(1, 15, 0)
#define DPT_Ack Dpt(1, 16, 0)
#define DPT_Trigger Dpt(1, 17, 0)
#define DPT_Occupancy Dpt(1, 18, 0)
#define DPT_Window_Door Dpt(1, 19, 0)
#define DPT_LogicalFunction Dpt(1, 21, 0)
#define DPT_Scene_AB Dpt(1, 22, 0)
#define DPT_ShutterBlinds_Mode Dpt(1, 23, 0)
#define DPT_Heat_Cool Dpt(1, 100, 0)
#define DPT_Switch_Control Dpt(2, 1, 0)
#define DPT_Bool_Control Dpt(2, 2, 0)
#define DPT_Enable_Control Dpt(2, 3, 0)
#define DPT_Ramp_Control Dpt(2, 4, 0)
#define DPT_Alarm_Control Dpt(2, 5, 0)
#define DPT_BinaryValue_Control Dpt(2, 6, 0)
#define DPT_Step_Control Dpt(2, 7, 0)
#define DPT_Direction1_Control Dpt(2, 8, 0)
#define DPT_Direction2_Control Dpt(2, 9, 0)
#define DPT_Start_Control Dpt(2, 10, 0)
#define DPT_State_Control Dpt(2, 11, 0)
#define DPT_Invert_Control Dpt(2, 12, 0)
#define DPT_Control_Dimming Dpt(3, 7, 0)
#define DPT_Control_Blinds Dpt(3, 8, 0)
#define DPT_Char_ASCII Dpt(4, 1, 0)
#define DPT_Char_8859_1 Dpt(4, 2, 0)
#define DPT_Scaling Dpt(5, 1, 0)
#define DPT_Angle Dpt(5, 3, 0)
#define DPT_Percent_U8 Dpt(5, 4, 0)
#define DPT_DecimalFactor Dpt(5, 5, 0)
#define DPT_Tariff Dpt(5, 6, 0)
#define DPT_Value_1_Ucount Dpt(5, 10, 0)
#define DPT_Percent_V8 Dpt(6, 1, 0)
#define DPT_Value_1_Count Dpt(6, 10, 0)
#define DPT_Status_Mode3 Dpt(6, 20, 0)
#define DPT_Value_2_Ucount Dpt(7, 1, 0)
#define DPT_TimePeriodMsec Dpt(7, 2, 0)
#define DPT_TimePeriod10MSec Dpt(7, 3, 0)
#define DPT_TimePeriod100MSec Dpt(7, 4, 0)
#define DPT_TimePeriodSec Dpt(7, 5, 0)
#define DPT_TimePeriodMin Dpt(7, 6, 0)
#define DPT_TimePeriodHrs Dpt(7, 7, 0)
#define DPT_PropDataType Dpt(7, 10, 0)
#define DPT_Length_mm Dpt(7, 11, 0)
#define DPT_UElCurrentmA Dpt(7, 12, 0)
#define DPT_Brightness Dpt(7, 13, 0)
#define DPT_Value_2_Count Dpt(8, 1, 0)
#define DPT_DeltaTimeMsec Dpt(8, 2, 0)
#define DPT_DeltaTime10MSec Dpt(8, 3, 0)
#define DPT_DeltaTime100MSec Dpt(8, 4, 0)
#define DPT_DeltaTimeSec Dpt(8, 5, 0)
#define DPT_DeltaTimeMin Dpt(8, 6, 0)
#define DPT_DeltaTimeHrs Dpt(8, 7, 0)
#define DPT_Percent_V16 Dpt(8, 10, 0)
#define DPT_Rotation_Angle Dpt(8, 11, 0)
#define DPT_Value_Temp Dpt(9, 1, 0)
#define DPT_Value_Tempd Dpt(9, 2, 0)
#define DPT_Value_Tempa Dpt(9, 3, 0)
#define DPT_Value_Lux Dpt(9, 4, 0)
#define DPT_Value_Wsp Dpt(9, 5, 0)
#define DPT_Value_Pres Dpt(9, 6, 0)
#define DPT_Value_Humidity Dpt(9, 7, 0)
#define DPT_Value_AirQuality Dpt(9, 8, 0)
#define DPT_Value_Time1 Dpt(9, 10, 0)
#define DPT_Value_Time2 Dpt(9, 11, 0)
#define DPT_Value_Volt Dpt(9, 20, 0)
#define DPT_Value_Curr Dpt(9, 21, 0)
#define DPT_PowerDensity Dpt(9, 22, 0)
#define DPT_KelvinPerPercent Dpt(9, 23, 0)
#define DPT_Power Dpt(9, 24, 0)
#define DPT_Value_Volume_Flow Dpt(9, 25, 0)
#define DPT_Rain_Amount Dpt(9, 26, 0)
#define DPT_Value_Temp_F Dpt(9, 27, 0)
#define DPT_Value_Wsp_kmh Dpt(9, 28, 0)
#define DPT_TimeOfDay Dpt(10, 1, 1, 0)
#define DPT_Date Dpt(11, 1, 0)
#define DPT_Value_4_Ucount Dpt(12, 1, 0)
#define DPT_Value_4_Count Dpt(13, 1, 0)
#define DPT_FlowRate_m3 Dpt(13, 2, 0) / h
#define DPT_ActiveEnergy Dpt(13, 10, 0)
#define DPT_ApparantEnergy Dpt(13, 11, 0)
#define DPT_ReactiveEnergy Dpt(13, 12, 0)
#define DPT_ActiveEnergy_kWh Dpt(13, 13, 0)
#define DPT_ApparantEnergy_kVAh Dpt(13, 14, 0)
#define DPT_ReactiveEnergy_kVARh Dpt(13, 15, 0)
#define DPT_LongDeltaTimeSec Dpt(13, 100, 0)
#define DPT_Value_Acceleration Dpt(14, 0, 0)
#define DPT_Value_Acceleration_Angular Dpt(14, 1, 0)
#define DPT_Value_Activation_Energy Dpt(14, 2, 0)
#define DPT_Value_Activity Dpt(14, 3, 0)
#define DPT_Value_Mol Dpt(14, 4, 0)
#define DPT_Value_Amplitude Dpt(14, 5, 0)
#define DPT_Value_AngleRad Dpt(14, 6, 0)
#define DPT_Value_AngleDeg Dpt(14, 7, 0)
#define DPT_Value_Angular_Momentum Dpt(14, 8, 0)
#define DPT_Value_Angular_Velocity Dpt(14, 9, 0)
#define DPT_Value_Area Dpt(14, 10, 0)
#define DPT_Value_Capacitance Dpt(14, 11, 0)
#define DPT_Value_Charge_DensitySurface Dpt(14, 12, 0)
#define DPT_Value_Charge_DensityVolume Dpt(14, 13, 0)
#define DPT_Value_Compressibility Dpt(14, 14, 0)
#define DPT_Value_Conductance Dpt(14, 15, 0)
#define DPT_Value_Electrical_Conductivity Dpt(14, 16, 0)
#define DPT_Value_Density Dpt(14, 17, 0)
#define DPT_Value_Electric_Charge Dpt(14, 18, 0)
#define DPT_Value_Electric_Current Dpt(14, 19, 0)
#define DPT_Value_Electric_CurrentDensity Dpt(14, 20, 0)
#define DPT_Value_Electric_DipoleMoment Dpt(14, 21, 0)
#define DPT_Value_Electric_Displacement Dpt(14, 22, 0)
#define DPT_Value_Electric_FieldStrength Dpt(14, 23, 0)
#define DPT_Value_Electric_Flux Dpt(14, 24, 0)
#define DPT_Value_Electric_FluxDensity Dpt(14, 25, 0)
#define DPT_Value_Electric_Polarization Dpt(14, 26, 0)
#define DPT_Value_Electric_Potential Dpt(14, 27, 0)
#define DPT_Value_Electric_PotentialDifference Dpt(14, 28, 0)
#define DPT_Value_ElectromagneticMoment Dpt(14, 29, 0)
#define DPT_Value_Electromotive_Force Dpt(14, 30, 0)
#define DPT_Value_Energy Dpt(14, 31, 0)
#define DPT_Value_Force Dpt(14, 32, 0)
#define DPT_Value_Frequency Dpt(14, 33, 0)
#define DPT_Value_Angular_Frequency Dpt(14, 34, 0)
#define DPT_Value_Heat_Capacity Dpt(14, 35, 0)
#define DPT_Value_Heat_FlowRate Dpt(14, 36, 0)
#define DPT_Value_Heat_Quantity Dpt(14, 37, 0)
#define DPT_Value_Impedance Dpt(14, 38, 0)
#define DPT_Value_Length Dpt(14, 39, 0)
#define DPT_Value_Light_Quantity Dpt(14, 40, 0)
#define DPT_Value_Luminance Dpt(14, 41, 0)
#define DPT_Value_Luminous_Flux Dpt(14, 42, 0)
#define DPT_Value_Luminous_Intensity Dpt(14, 43, 0)
#define DPT_Value_Magnetic_FieldStrength Dpt(14, 44, 0)
#define DPT_Value_Magnetic_Flux Dpt(14, 45, 0)
#define DPT_Value_Magnetic_FluxDensity Dpt(14, 46, 0)
#define DPT_Value_Magnetic_Moment Dpt(14, 47, 0)
#define DPT_Value_Magnetic_Polarization Dpt(14, 48, 0)
#define DPT_Value_Magnetization Dpt(14, 49, 0)
#define DPT_Value_MagnetomotiveForce Dpt(14, 50, 0)
#define DPT_Value_Mass Dpt(14, 51, 0)
#define DPT_Value_MassFlux Dpt(14, 52, 0)
#define DPT_Value_Momentum Dpt(14, 53, 0)
#define DPT_Value_Phase_AngleRad Dpt(14, 54, 0)
#define DPT_Value_Phase_AngleDeg Dpt(14, 55, 0)
#define DPT_Value_Power Dpt(14, 56, 0)
#define DPT_Value_Power_Factor Dpt(14, 57, 0)
#define DPT_Value_Pressure Dpt(14, 58, 0)
#define DPT_Value_Reactance Dpt(14, 59, 0)
#define DPT_Value_Resistance Dpt(14, 60, 0)
#define DPT_Value_Resistivity Dpt(14, 61, 0)
#define DPT_Value_SelfInductance Dpt(14, 62, 0)
#define DPT_Value_SolidAngle Dpt(14, 63, 0)
#define DPT_Value_Sound_Intensity Dpt(14, 64, 0)
#define DPT_Value_Speed Dpt(14, 65, 0)
#define DPT_Value_Stress Dpt(14, 66, 0)
#define DPT_Value_Surface_Tension Dpt(14, 67, 0)
#define DPT_Value_Common_Temperature Dpt(14, 68, 0)
#define DPT_Value_Absolute_Temperature Dpt(14, 69, 0)
#define DPT_Value_TemperatureDifference Dpt(14, 70, 0)
#define DPT_Value_Thermal_Capacity Dpt(14, 71, 0)
#define DPT_Value_Thermal_Conductivity Dpt(14, 72, 0)
#define DPT_Value_ThermoelectricPower Dpt(14, 73, 0)
#define DPT_Value_Time Dpt(14, 74, 0)
#define DPT_Value_Torque Dpt(14, 75, 0)
#define DPT_Value_Volume Dpt(14, 76, 0)
#define DPT_Value_Volume_Flux Dpt(14, 77, 0)
#define DPT_Value_Weight Dpt(14, 78, 0)
#define DPT_Value_Work Dpt(14, 79, 0)
#define DPT_Value_ApparentPower Dpt(14, 80, 0)
#define DPT_Access_Data Dpt(15, 0, 0)
#define DPT_String_ASCII Dpt(16, 0, 0)
#define DPT_String_8859_1 Dpt(16, 1, 0)
#define DPT_SceneNumber Dpt(17, 1, 0)
#define DPT_SceneControl Dpt(18, 1, 0)
#define DPT_DateTime Dpt(19, 1, 0)
#define DPT_SCLOMode Dpt(20, 1, 0)
#define DPT_BuildingMode Dpt(20, 2, 0)
#define DPT_OccMode Dpt(20, 3, 0)
#define DPT_Priority Dpt(20, 4, 0)
#define DPT_LightApplicationMode Dpt(20, 5, 0)
#define DPT_ApplicationArea Dpt(20, 6, 0)
#define DPT_AlarmClassType Dpt(20, 7, 0)
#define DPT_PSUMode Dpt(20, 8, 0)
#define DPT_ErrorClass_System Dpt(20, 11, 0)
#define DPT_ErrorClass_HVAC Dpt(20, 12, 0)
#define DPT_Time_Delay Dpt(20, 13, 0)
#define DPT_Beaufort_Wind_Force_Scale Dpt(20, 14, 0)
#define DPT_SensorSelect Dpt(20, 17, 0)
#define DPT_ActuatorConnectType Dpt(20, 20, 0)
#define DPT_FuelType Dpt(20, 100, 0)
#define DPT_BurnerType Dpt(20, 101, 0)
#define DPT_HVACMode Dpt(20, 102, 0)
#define DPT_DHWMode Dpt(20, 103, 0)
#define DPT_LoadPriority Dpt(20, 104, 0)
#define DPT_HVACContrMode Dpt(20, 105, 0)
#define DPT_HVACEmergMode Dpt(20, 106, 0)
#define DPT_ChangeoverMode Dpt(20, 107, 0)
#define DPT_ValveMode Dpt(20, 108, 0)
#define DPT_DamperMode Dpt(20, 109, 0)
#define DPT_HeaterMode Dpt(20, 110, 0)
#define DPT_FanMode Dpt(20, 111, 0)
#define DPT_MasterSlaveMode Dpt(20, 112, 0)
#define DPT_StatusRoomSetp Dpt(20, 113, 0)
#define DPT_ADAType Dpt(20, 120, 0)
#define DPT_BackupMode Dpt(20, 121, 0)
#define DPT_StartSynchronization Dpt(20, 122, 0)
#define DPT_Behaviour_Lock_Unlock Dpt(20, 600, 0)
#define DPT_Behaviour_Bus_Power_Up_Down Dpt(20, 601, 0)
#define DPT_DALI_Fade_Time Dpt(20, 602, 0)
#define DPT_BlinkingMode Dpt(20, 603, 0)
#define DPT_LightControlMode Dpt(20, 604, 0)
#define DPT_SwitchPBModel Dpt(20, 605, 0)
#define DPT_PBAction Dpt(20, 606, 0)
#define DPT_DimmPBModel Dpt(20, 607, 0)
#define DPT_SwitchOnMode Dpt(20, 608, 0)
#define DPT_LoadTypeSet Dpt(20, 609, 0)
#define DPT_LoadTypeDetected Dpt(20, 610, 0)
#define DPT_SABExceptBehaviour Dpt(20, 801, 0)
#define DPT_SABBehaviour_Lock_Unlock Dpt(20, 802, 0)
#define DPT_SSSBMode Dpt(20, 803, 0)
#define DPT_BlindsControlMode Dpt(20, 804, 0)
#define DPT_CommMode Dpt(20, 1000, 0)
#define DPT_AddInfoTypes Dpt(20, 1001, 0)
#define DPT_RF_ModeSelect Dpt(20, 1002, 0)
#define DPT_RF_FilterSelect Dpt(20, 1003, 0)
#define DPT_StatusGen Dpt(21, 1, 0)
#define DPT_Device_Control Dpt(21, 2, 0)
#define DPT_ForceSign Dpt(21, 100, 0)
#define DPT_ForceSignCool Dpt(21, 101, 0)
#define DPT_StatusRHC Dpt(21, 102, 0)
#define DPT_StatusSDHWC Dpt(21, 103, 0)
#define DPT_FuelTypeSet Dpt(21, 104, 0)
#define DPT_StatusRCC Dpt(21, 105, 0)
#define DPT_StatusAHU Dpt(21, 106, 0)
#define DPT_LightActuatorErrorInfo Dpt(21, 601, 0)
#define DPT_RF_ModeInfo Dpt(21, 1000, 0)
#define DPT_RF_FilterInfo Dpt(21, 1001, 0)
#define DPT_Channel_Activation_8 Dpt(21, 1010, 0)
#define DPT_StatusDHWC Dpt(22, 100, 0)
#define DPT_StatusRHCC Dpt(22, 101, 0)
#define DPT_Media Dpt(22, 1000, 0)
#define DPT_Channel_Activation_16 Dpt(22, 1010, 0)
#define DPT_OnOff_Action Dpt(23, 1, 0)
#define DPT_Alarm_Reaction Dpt(23, 2, 0)
#define DPT_UpDown_Action Dpt(23, 3, 0)
#define DPT_HVAC_PB_Action Dpt(23, 102, 0)
#define DPT_VarString_8859_1 Dpt(24, 1, 0)
#define DPT_DoubleNibble Dpt(25, 1000, 0)
#define DPT_SceneInfo Dpt(26, 1, 0)
#define DPT_CombinedInfoOnOff Dpt(27, 1, 0)
#define DPT_UTF_8 Dpt(28, 1, 0)
#define DPT_ActiveEnergy_V64 Dpt(29, 10, 0)
#define DPT_ApparantEnergy_V64 Dpt(29, 11, 0)
#define DPT_ReactiveEnergy_V64 Dpt(29, 12, 0)
#define DPT_Channel_Activation_24 Dpt(30, 1010, 0)
#define DPT_PB_Action_HVAC_Extended Dpt(31, 101, 0)
#define DPT_Heat_Cool_Z Dpt(200, 100, 0)
#define DPT_BinaryValue_Z Dpt(200, 101, 0)
#define DPT_HVACMode_Z Dpt(201, 100, 0)
#define DPT_DHWMode_Z Dpt(201, 102, 0)
#define DPT_HVACContrMode_Z Dpt(201, 104, 0)
#define DPT_EnablH_Cstage_Z Dpt(201, 105, 0)
#define DPT_BuildingMode_Z Dpt(201, 107, 0)
#define DPT_OccMode_Z Dpt(201, 108, 0)
#define DPT_HVACEmergMode_Z Dpt(201, 109, 0)
#define DPT_RelValue_Z Dpt(202, 1, 0)
#define DPT_UCountValue8_Z Dpt(202, 2, 0)
#define DPT_TimePeriodMsec_Z Dpt(203, 2, 0)
#define DPT_TimePeriod10Msec_Z Dpt(203, 3, 0)
#define DPT_TimePeriod100Msec_Z Dpt(203, 4, 0)
#define DPT_TimePeriodSec_Z Dpt(203, 5, 0)
#define DPT_TimePeriodMin_Z Dpt(203, 6, 0)
#define DPT_TimePeriodHrs_Z Dpt(203, 7, 0)
#define DPT_UFlowRateLiter_per_h_Z Dpt(203, 11, 0)
#define DPT_UCountValue16_Z Dpt(203, 12, 0)
#define DPT_UElCurrent_Z Dpt(203, 13, 0) 
#define DPT_PowerKW_Z Dpt(203, 14, 0)
#define DPT_AtmPressureAbs_Z Dpt(203, 15, 0)
#define DPT_PercentU16_Z Dpt(203, 17, 0)
#define DPT_HVACAirQual_Z Dpt(203, 100, 0)
#define DPT_WindSpeed_Z Dpt(203, 101, 0)
#define DPT_SunIntensity_Z Dpt(203, 102, 0)
#define DPT_HVACAirFlowAbs_Z Dpt(203, 104, 0)
#define DPT_RelSignedValue_Z Dpt(204, 1, 0)
#define DPT_DeltaTimeMsec_Z Dpt(205, 2, 0)
#define DPT_DeltaTime10Msec_Z Dpt(205, 3, 0)
#define DPT_DeltaTime100Msec_Z Dpt(205, 4, 0)
#define DPT_DeltaTimeSec_Z Dpt(205, 5, 0)
#define DPT_DeltaTimeMin_Z Dpt(205, 6, 0)
#define DPT_DeltaTimeHrs_Z Dpt(205, 7, 0)
#define DPT_Percent_V16_Z Dpt(205, 17, 0)
#define DPT_TempHVACAbs_Z Dpt(205, 100, 0)
#define DPT_TempHVACRel_Z Dpt(205, 101, 0)
#define DPT_HVACAirFlowRel_Z Dpt(205, 102, 0)
#define DPT_HVACModeNext Dpt(206, 100, 0)
#define DPT_DHWModeNext Dpt(206, 102, 0)
#define DPT_OccModeNext Dpt(206, 104, 0)
#define DPT_BuildingModeNext Dpt(206, 105, 0)
#define DPT_StatusBUC Dpt(207, 100, 0)
#define DPT_LockSign Dpt(207, 101, 0)
#define DPT_ValueDemBOC Dpt(207, 102, 0)
#define DPT_ActPosDemAbs Dpt(207, 104, 0)
#define DPT_StatusAct Dpt(207, 105, 0)
#define DPT_StatusLightingActuator Dpt(207, 600, 0)
#define DPT_StatusHPM Dpt(209, 100, 0)
#define DPT_TempRoomDemAbs Dpt(209, 101, 0)
#define DPT_StatusCPM Dpt(209, 102, 0)
#define DPT_StatusWTC Dpt(209, 103, 0)
#define DPT_TempFlowWaterDemAbs Dpt(210, 100, 0)
#define DPT_EnergyDemWater Dpt(211, 100, 0)
#define DPT_TempRoomSetpSetShift_3 Dpt(212, 100, 0)
#define DPT_TempRoomSetpSet_3 Dpt(212, 101, 0)
#define DPT_TempRoomSetpSet_4 Dpt(213, 100, 0)
#define DPT_TempDHWSetpSet_4 Dpt(213, 101, 0)
#define DPT_TempRoomSetpSetShift_4 Dpt(213, 102, 0)
#define DPT_PowerFlowWaterDemHPM Dpt(214, 100, 0)
#define DPT_PowerFlowWaterDemCPM Dpt(214, 101, 0)
#define DPT_StatusBOC Dpt(215, 100, 0)
#define DPT_StatusCC Dpt(215, 101, 0)
#define DPT_SpecHeatProd Dpt(216, 100, 0)
#define DPT_Version Dpt(217, 1, 0)
#define DPT_VolumeLiter_Z Dpt(218, 1, 0)
#define DPT_FlowRate_m3_per_h_Z Dpt(218, 2, 0)
#define DPT_AlarmInfo Dpt(219, 1, 0)
#define DPT_TempHVACAbsNext Dpt(220, 100, 0)
#define DPT_SerNum Dpt(221, 1, 0)
#define DPT_TempRoomSetpSetF16_3 Dpt(222, 100, 0)
#define DPT_TempRoomSetpSetShiftF16_3 Dpt(222, 101, 0)
#define DPT_EnergyDemAir Dpt(223, 100, 0)
#define DPT_TempSupplyAirSetpSet Dpt(224, 100, 0)
#define DPT_ScalingSpeed Dpt(225, 1, 0)
#define DPT_Scaling_Step_Time Dpt(225, 2, 0)
#define DPT_TariffNext Dpt(225, 3, 0)
#define DPT_MeteringValue Dpt(229, 1, 0)
#define DPT_MBus_Address Dpt(230, 1000, 0)
#define DPT_Locale_ASCII Dpt(231, 1, 0)
#define DPT_Colour_RGB Dpt(232, 600, 0)
#define DPT_LanguageCodeAlpha2_ASCII Dpt(234, 1, 0)
#define DPT_RegionCodeAlpha2_ASCII Dpt(234, 2, 0)
#define DPT_Tariff_ActiveEnergy Dpt(235, 1, 0)
#define DPT_Prioritised_Mode_Control Dpt(236, 1, 0)
#define DPT_DALI_Control_Gear_Diagnostic Dpt(237, 600, 0)
#define DPT_SceneConfig Dpt(238, 1, 0)
#define DPT_DALI_Diagnostics Dpt(238, 600, 0)
#define DPT_FlaggedScaling Dpt(239, 1, 0)
#define DPT_CombinedPosition Dpt(240, 800, 0)
#define DPT_StatusSAB Dpt(241, 800, 0)
#define DPT_Colour_RGBW Dpt(251, 600, 0)

class Dpt
{
  public:

    
    unsigned short mainGroup;
    unsigned short subGroup;
    unsigned short index;

    Dpt();
    Dpt(short _mainGroup, short _subGroup, short _index);
    bool operator==(const Dpt& other) const;
    bool operator!=(const Dpt& other) const;
};
