#ifndef _FREQ_H
#define _FREQ_H  1

// musical frequency definitions, assuming A4 = 440 Hz

#ifndef PWM_SCALER
#  define PWM_SCALER  256
#endif
#ifndef PWM_FREQ
#  define PWM_FREQ  F_CPU / PWM_SCALER
#endif
#define TMP_FREQ  PWM_FREQ * 10

//
// freq. (*10) definitions
//

#define FREQ_C_0		164
#define FREQ_Db0		173
#define FREQ_D_0		184
#define FREQ_Eb0		194
#define FREQ_E_0		206
#define FREQ_F_0		218
#define FREQ_Gb0		231
#define FREQ_G_0		245
#define FREQ_Ab0		260
#define FREQ_A_0		275
#define FREQ_Bb0		291
#define FREQ_B_0		309
#define FREQ_C_1		327
#define FREQ_Db1		346
#define FREQ_D_1		367
#define FREQ_Eb1		389
#define FREQ_E_1		412
#define FREQ_F_1		437
#define FREQ_Gb1		462
#define FREQ_G_1		490
#define FREQ_Ab1		519
#define FREQ_A_1		550
#define FREQ_Bb1		583
#define FREQ_B_1		617
#define FREQ_C_2		654
#define FREQ_Db2		693
#define FREQ_D_2		734
#define FREQ_Eb2		778
#define FREQ_E_2		824
#define FREQ_F_2		873
#define FREQ_Gb2		925
#define FREQ_G_2		980
#define FREQ_Ab2		1038
#define FREQ_A_2		1100
#define FREQ_Bb2		1165
#define FREQ_B_2		1235
#define FREQ_C_3		1308
#define FREQ_Db3		1386
#define FREQ_D_3		1468
#define FREQ_Eb3		1556
#define FREQ_E_3		1648
#define FREQ_F_3		1746
#define FREQ_Gb3		1850
#define FREQ_G_3		1960
#define FREQ_Ab3		2077
#define FREQ_A_3		2200
#define FREQ_Bb3		2331
#define FREQ_B_3		2469
#define FREQ_C_4		2616
#define FREQ_Db4		2772
#define FREQ_D_4		2937
#define FREQ_Eb4		3111
#define FREQ_E_4		3296
#define FREQ_F_4		3492
#define FREQ_Gb4		3700
#define FREQ_G_4		3920
#define FREQ_Ab4		4153
#define FREQ_A_4		4400
#define FREQ_Bb4		4662
#define FREQ_B_4		4939
#define FREQ_C_5		5233
#define FREQ_Db5		5544
#define FREQ_D_5		5873
#define FREQ_Eb5		6223
#define FREQ_E_5		6593
#define FREQ_F_5		6985
#define FREQ_Gb5		7400
#define FREQ_G_5		7840
#define FREQ_Ab5		8306
#define FREQ_A_5		8800
#define FREQ_Bb5		9323
#define FREQ_B_5		9878
#define FREQ_C_6		10465
#define FREQ_Db6		11087
#define FREQ_D_6		11747
#define FREQ_Eb6		12445
#define FREQ_E_6		13185
#define FREQ_F_6		13969
#define FREQ_Gb6		14800
#define FREQ_G_6		15680
#define FREQ_Ab6		16612
#define FREQ_A_6		17600
#define FREQ_Bb6		18647
#define FREQ_B_6		19755
#define FREQ_C_7		20930
#define FREQ_Db7		22175
#define FREQ_D_7		23493

//
// half-cycle length definitions
//

#define FREQ2CYCLES(x)  ( ( TMP_FREQ + x / 2 ) / x )

#define C_0		FREQ2CYCLES( FREQ_C_0 )
#define Db0		FREQ2CYCLES( FREQ_Db0 )
#define D_0		FREQ2CYCLES( FREQ_D_0 )
#define Eb0		FREQ2CYCLES( FREQ_Eb0 )
#define E_0		FREQ2CYCLES( FREQ_E_0 )
#define F_0		FREQ2CYCLES( FREQ_F_0 )
#define Gb0		FREQ2CYCLES( FREQ_Gb0 )
#define G_0		FREQ2CYCLES( FREQ_G_0 )
#define Ab0		FREQ2CYCLES( FREQ_Ab0 )
#define A_0		FREQ2CYCLES( FREQ_A_0 )
#define Bb0		FREQ2CYCLES( FREQ_Bb0 )
#define B_0		FREQ2CYCLES( FREQ_B_0 )
#define C_1		FREQ2CYCLES( FREQ_C_1 )
#define Db1		FREQ2CYCLES( FREQ_Db1 )
#define D_1		FREQ2CYCLES( FREQ_D_1 )
#define Eb1		FREQ2CYCLES( FREQ_Eb1 )
#define E_1		FREQ2CYCLES( FREQ_E_1 )
#define F_1		FREQ2CYCLES( FREQ_F_1 )
#define Gb1		FREQ2CYCLES( FREQ_Gb1 )
#define G_1		FREQ2CYCLES( FREQ_G_1 )
#define Ab1		FREQ2CYCLES( FREQ_Ab1 )
#define A_1		FREQ2CYCLES( FREQ_A_1 )
#define Bb1		FREQ2CYCLES( FREQ_Bb1 )
#define B_1		FREQ2CYCLES( FREQ_B_1 )
#define C_2		FREQ2CYCLES( FREQ_C_2 )
#define Db2		FREQ2CYCLES( FREQ_Db2 )
#define D_2		FREQ2CYCLES( FREQ_D_2 )
#define Eb2		FREQ2CYCLES( FREQ_Eb2 )
#define E_2		FREQ2CYCLES( FREQ_E_2 )
#define F_2		FREQ2CYCLES( FREQ_F_2 )
#define Gb2		FREQ2CYCLES( FREQ_Gb2 )
#define G_2		FREQ2CYCLES( FREQ_G_2 )
#define Ab2		FREQ2CYCLES( FREQ_Ab2 )
#define A_2		FREQ2CYCLES( FREQ_A_2 )
#define Bb2		FREQ2CYCLES( FREQ_Bb2 )
#define B_2		FREQ2CYCLES( FREQ_B_2 )
#define C_3		FREQ2CYCLES( FREQ_C_3 )
#define Db3		FREQ2CYCLES( FREQ_Db3 )
#define D_3		FREQ2CYCLES( FREQ_D_3 )
#define Eb3		FREQ2CYCLES( FREQ_Eb3 )
#define E_3		FREQ2CYCLES( FREQ_E_3 )
#define F_3		FREQ2CYCLES( FREQ_F_3 )
#define Gb3		FREQ2CYCLES( FREQ_Gb3 )
#define G_3		FREQ2CYCLES( FREQ_G_3 )
#define Ab3		FREQ2CYCLES( FREQ_Ab3 )
#define A_3		FREQ2CYCLES( FREQ_A_3 )
#define Bb3		FREQ2CYCLES( FREQ_Bb3 )
#define B_3		FREQ2CYCLES( FREQ_B_3 )
#define C_4		FREQ2CYCLES( FREQ_C_4 )
#define Db4		FREQ2CYCLES( FREQ_Db4 )
#define D_4		FREQ2CYCLES( FREQ_D_4 )
#define Eb4		FREQ2CYCLES( FREQ_Eb4 )
#define E_4		FREQ2CYCLES( FREQ_E_4 )
#define F_4		FREQ2CYCLES( FREQ_F_4 )
#define Gb4		FREQ2CYCLES( FREQ_Gb4 )
#define G_4		FREQ2CYCLES( FREQ_G_4 )
#define Ab4		FREQ2CYCLES( FREQ_Ab4 )
#define A_4		FREQ2CYCLES( FREQ_A_4 )
#define Bb4		FREQ2CYCLES( FREQ_Bb4 )
#define B_4		FREQ2CYCLES( FREQ_B_4 )
#define C_5		FREQ2CYCLES( FREQ_C_5 )
#define Db5		FREQ2CYCLES( FREQ_Db5 )
#define D_5		FREQ2CYCLES( FREQ_D_5 )
#define Eb5		FREQ2CYCLES( FREQ_Eb5 )
#define E_5		FREQ2CYCLES( FREQ_E_5 )
#define F_5		FREQ2CYCLES( FREQ_F_5 )
#define Gb5		FREQ2CYCLES( FREQ_Gb5 )
#define G_5		FREQ2CYCLES( FREQ_G_5 )
#define Ab5		FREQ2CYCLES( FREQ_Ab5 )
#define A_5		FREQ2CYCLES( FREQ_A_5 )
#define Bb5		FREQ2CYCLES( FREQ_Bb5 )
#define B_5		FREQ2CYCLES( FREQ_B_5 )
#define C_6		FREQ2CYCLES( FREQ_C_6 )
#define Db6		FREQ2CYCLES( FREQ_Db6 )
#define D_6		FREQ2CYCLES( FREQ_D_6 )
#define Eb6		FREQ2CYCLES( FREQ_Eb6 )
#define E_6		FREQ2CYCLES( FREQ_E_6 )
#define F_6		FREQ2CYCLES( FREQ_F_6 )
#define Gb6		FREQ2CYCLES( FREQ_Gb6 )
#define G_6		FREQ2CYCLES( FREQ_G_6 )
#define Ab6		FREQ2CYCLES( FREQ_Ab6 )
#define A_6		FREQ2CYCLES( FREQ_A_6 )
#define Bb6		FREQ2CYCLES( FREQ_Bb6 )
#define B_6		FREQ2CYCLES( FREQ_B_6 )
#define C_7		FREQ2CYCLES( FREQ_C_7 )
#define Db7		FREQ2CYCLES( FREQ_Db7 )
#define D_7		FREQ2CYCLES( FREQ_D_7 )

#endif // _FREQ_H