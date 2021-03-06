/*
*
* Copyright (c) 2018 ERA Instruments (http://erainstruments.com/)

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

void setFreqParam(uint64_t freq)
{
	uint8_t CHDIV				= 0;
	uint16_t alpha				= 0;
	double FVCO2				= 0;
	double FVCO1				= 0;
	uint32_t N_LMX2				= 0;
	uint32_t WBFM_N_LMX2		= 0;
	double Fref					= 0;
	double N_LMX1				= 0;
	uint32_t N_LMX1_Int			= 0;
	double N_LMX1_Frac			= 0;
	uint32_t LMX1_PLL_DEN		= 0;
	uint32_t LMX1_PLL_NUM		= 0;
	uint32_t LMX1_PLL_NUM_MSB	= 0;
	uint32_t LMX1_PLL_NUM_LSB	= 0;
	uint32_t DDS_FTW			= 0;
	double Fpfd1				= 200e6;
	double Fpfd2				= 267e6;
	int loop_count				= 0;
	LMX2_R45_update				= 0x2DC0C0; //OUTA_MUX = 0
	LMX2_R27_update				= 0x1B0002; //VCO2X_EN = 0

	if (freq > 15000e6) 
	{
		ODIV = 0.5;
		LMX2_R45_update = 0x2DD0C0; //OUTA_MUX = 2
		LMX2_R27_update = 0x1B0003; //VCO2X_EN = 1
	}
	else if (freq > 7500e6 && freq <= 15000e6) 
	{
		ODIV = 1;
		LMX2_R45_update = 0x2DC8C0; //OUTA_MUX = 1
	}
	else if (freq > 3750e6 && freq <= 7500e6) 
	{
		ODIV = 2;
		CHDIV = 0;
	}
	else if (freq > 1875e6 && freq <= 3750e6) 
	{
		ODIV = 4;
		CHDIV = 1;
	}
	else if (freq > 1400e6 && freq <= 1875e6) 
	{
		ODIV = 6;
		CHDIV = 2;
	}
	else if (freq > 950e6 && freq <= 1400e6) 
	{
		ODIV = 8;
		CHDIV = 3;
	}
	else if (freq > 700e6 && freq <= 950e6) 
	{
		ODIV = 12;
		CHDIV = 4;
	}
	else if (freq > 470e6 && freq <= 700e6) 
	{
		ODIV = 16;
		CHDIV = 5;
	}
	else if (freq > 350e6 && freq <= 470e6) 
	{
		ODIV = 24;
		CHDIV = 6;
	}
	else if (freq > 235e6 && freq <= 350e6) 
	{
		ODIV = 32;
		CHDIV = 7;
	}
	else if (freq >175e6 && freq <= 235e6) 
	{
		ODIV = 48;
		CHDIV = 8;
	}
	else if (freq > 119e6 && freq <= 175e6) 
	{
		ODIV = 64;
		CHDIV = 9;
	}
	else if (freq > 80e6 && freq <= 119e6)
	{
		ODIV = 96;
		CHDIV = 11;
	}
	else if (freq > 59e6 && freq <= 80e6) 
	{
		ODIV = 128;
		CHDIV = 12;
	}
	else if (freq > 40e6 && freq <= 59e6)
	{
		ODIV = 192;
		CHDIV = 13;
	}
	else if (freq > 30e6 && freq <= 40e6) 
	{
		ODIV = 256;
		CHDIV = 14;
	}
	else if (freq >= 0.25e6 && freq <= 30e6) 
	{
		spiWrite_LMX(&LMX1_R75_DDS, LMX1_LE);
		spiWrite_LMX(&LMX1_R36_DDS, LMX1_LE);
		spiWrite_LMX(&LMX1_R43_DDS, LMX1_LE);
		spiWrite_LMX(&LMX1_R42_DDS, LMX1_LE);
		spiWrite_LMX(&LMX1_R39_DDS, LMX1_LE);
		spiWrite_LMX(&LMX1_R38_DDS, LMX1_LE);
		spiWrite_LMX(&LMX1_R0, LMX1_LE);

		DDS_FTW = 16.777216 * freq;
		UINT32to4Bytes_DDS(DDS_FTW_update, &DDS_FTW);
		digitalWrite(DDS_IO_UPD, LOW);
		spiWrite(DDS_FTW_update, 5, DDS_LE);
		digitalWrite(DDS_IO_UPD, HIGH);
		return;
	}

	LMX2_R75_update = 64 * CHDIV + 4917248;
	FVCO2 = freq * ODIV;

	N_LMX2 = floor(FVCO2 / Fpfd2);
	LMX2_R36_update = N_LMX2 + 2359296;
	Fref = FVCO2 / (double)N_LMX2;

	if (FVCO2 > 12500e6)
	{
		LMX2_PFD_DLY_SEL = 1;
		LMX2_R37_update = LMX2_R37_update_1;
	}
	else
	{
		LMX2_PFD_DLY_SEL = 2;
		LMX2_R37_update = LMX2_R37_update_2;
	}

	FVCO1			= Fref * 32;
	N_LMX1			= FVCO1 / Fpfd1;
	N_LMX1_Int		= floor(N_LMX1);
	N_LMX1_Frac		= N_LMX1 - N_LMX1_Int;
	LMX1_R36_update = N_LMX1_Int + 2359296;

	while (N_LMX1_Frac <= 0.016 || (N_LMX1_Frac >= 0.484 && N_LMX1_Frac <= 0.516) || N_LMX1_Frac >= 0.984 || (N_LMX1_Frac >= 0.234 && N_LMX1_Frac <= 0.266) || (N_LMX1_Frac >= 0.734 && N_LMX1_Frac <= 0.766) || (N_LMX1_Frac >= 0.650 && N_LMX1_Frac <= 0.682) || (N_LMX1_Frac >= 0.317 && N_LMX1_Frac <= 0.349)) 
	{
		Fpfd2 = Fpfd2 - 1e6;

		N_LMX2 = floor(FVCO2 / Fpfd2);

		LMX2_R36_update = N_LMX2 + 2359296;
		Fref = FVCO2 / (double)N_LMX2;

		FVCO1 = Fref * 32;
		N_LMX1 = FVCO1 / Fpfd1;
		N_LMX1_Int = floor(N_LMX1);
		N_LMX1_Frac = N_LMX1 - N_LMX1_Int;
		LMX1_R36_update = N_LMX1_Int + 2359296;

		if (loop_count++ == 20) { break; }
	}
	
	
	if (!is_modulation_stopped && modType == WBFM_Mod) 
	{
		WBFM_N_LMX2 = N_LMX2 * 2000;
		if (WBFM_N_LMX2 >= 65536) 
		{
			WBFM_LMX2_R34_update = 2228225;
			WBFM_LMX2_R36_update = WBFM_N_LMX2 + 2293760;
		}
		else 
		{
			WBFM_LMX2_R34_update = 2228224;
			WBFM_LMX2_R36_update = WBFM_N_LMX2 + 2359296;
		}
	}

	LMX1_PLL_DEN	= 4294967295;
	LMX1_R38_update = (LMX1_PLL_DEN >> 16) + 2490368;			//DEN_MSB
	LMX1_R39_update = (LMX1_PLL_DEN & 0x0000FFFF) + 2555904;	//DEN_LSB
	LMX1_PLL_NUM	= floor((N_LMX1_Frac * LMX1_PLL_DEN) + 0.5);
	LMX1_R42_update = (LMX1_PLL_NUM >> 16) + 2752512;			//NUM_MSB
	LMX1_R43_update = (LMX1_PLL_NUM & 0x0000FFFF) + 2818048;	//NUM_LSB

	spiWrite_LMX(&LMX1_R36_update, LMX1_LE);
	spiWrite_LMX(&LMX1_R75, LMX1_LE);
	spiWrite_LMX(&LMX1_R43_update, LMX1_LE);
	spiWrite_LMX(&LMX1_R42_update, LMX1_LE);
	spiWrite_LMX(&LMX1_R39_update, LMX1_LE);
	spiWrite_LMX(&LMX1_R38_update, LMX1_LE);
	spiWrite_LMX(&LMX1_R0, LMX1_LE);

	delayMicroseconds(100);

	if (previous_LMX2_PFD_DLY_SEL != LMX2_PFD_DLY_SEL) 
	{
		spiWrite_LMX(&LMX2_R37_update, LMX2_LE);
		previous_LMX2_PFD_DLY_SEL = LMX2_PFD_DLY_SEL;
	}


	if (!is_modulation_stopped && modType == WBFM_Mod && !is_sweep_stopped) 
	{
		digitalWrite(WB_FM_En, LOW); //WB_FM Disabled. Normal Operation
		spiWrite_LMX(&LMX2_register[112 - 12], LMX2_LE);
		spiWrite_LMX(&LMX2_register[112 - 11], LMX2_LE);
		spiWrite_LMX(&LMX2_register[112 - 34], LMX2_LE);
		spiWrite_LMX(&LMX2_register[112 - 14], LMX2_LE);
	}

	spiWrite_LMX(&LMX2_R36_update, LMX2_LE);
	spiWrite_LMX(&LMX2_R75_update, LMX2_LE);
	spiWrite_LMX(&LMX2_R45_update, LMX2_LE);
	spiWrite_LMX(&LMX2_R27_update, LMX2_LE);
	spiWrite_LMX(&LMX2_R0, LMX2_LE);

	if (is_sweep_stopped) 
	{ 
		// if ERASynth is not in sweep mode
		delayMicroseconds(500);
		spiWrite_LMX(&LMX2_R0, LMX2_LE);
	}

	if (!is_modulation_stopped && modType == WBFM_Mod && !is_sweep_stopped) 
	{
		// It is required when changing the frequency while modulation is ON.
		delayMicroseconds(500); 
		spiWrite_LMX(&WBFM_LMX2_R12, LMX2_LE);
		spiWrite_LMX(&WBFM_LMX2_R11, LMX2_LE);
		spiWrite_LMX(&WBFM_LMX2_R34_update, LMX2_LE);
		spiWrite_LMX(&WBFM_LMX2_R36_update, LMX2_LE);
		spiWrite_LMX(&WBFM_LMX2_R14, LMX2_LE);
		digitalWrite(WB_FM_En, HIGH); //WB_FM Enabled.
	}
}