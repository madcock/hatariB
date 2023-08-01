/*
  Hatari - low640x16_spec.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Screen conversion, Low Res Spec512 to 640x16Bit
*/

static void ConvertLowRes_640x16Bit_Spec(void)
{
	Uint16 *PCScreen = (Uint16 *)pPCScreenDest;
	Uint32 *edi, *ebp;
	Uint32 *esi;
	Uint32 eax;
	int y;

	Spec512_StartFrame();            /* Start frame, track palettes */

	for (y = STScreenStartHorizLine; y < STScreenEndHorizLine; y++)
	{
		eax = STScreenLineOffset[y] + STScreenLeftSkipBytes;  /* Offset for this line + Amount to skip on left hand side */
		edi = (Uint32 *)((Uint8 *)pSTScreen + eax);        /* ST format screen 4-plane 16 colors */
		ebp = (Uint32 *)((Uint8 *)pSTScreenCopy + eax);    /* Previous ST format screen */
		esi = (Uint32 *)PCScreen;                          /* PC format screen */

		Line_ConvertLowRes_640x16Bit_Spec(edi, ebp, esi, eax);

		PCScreen = Double_ScreenLine16(PCScreen, PCScreenBytesPerLine);
	}

        bScreenContentsChanged = true;
}


static void Line_ConvertLowRes_640x16Bit_Spec(Uint32 *edi, Uint32 *ebp, Uint32 *esi, Uint32 eax)
{
	int x;
	Uint32 ebx, ecx, edx;
	Uint32 pixelspace[5]; /* Workspace to store pixels to so can print in right order for Spec512 */

	/* on x86, unaligned access macro touches also
	 * next byte, zero it for code checkers
	 */
	pixelspace[4] = 0;

	Spec512_StartScanLine();        /* Build up palettes for every 4 pixels, store in 'ScanLinePalettes' */

	x = STScreenWidthBytes >> 3;   /* Amount to draw across in 16-pixels (8 bytes) */

	do  /* x-loop */
	{
		ebx = *edi;                 /* Do 16 pixels at one time */
		ecx = *(edi+1);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		/* Convert planes to byte indices - as works in wrong order store to workspace so can read back in order! */
		LOW_BUILD_PIXELS_0 ;        /* Generate 'ecx' as pixels [12,13,14,15] */
		pixelspace[3] = ecx;
		LOW_BUILD_PIXELS_1 ;        /* Generate 'ecx' as pixels [4,5,6,7] */
		pixelspace[1] = ecx;
		LOW_BUILD_PIXELS_2 ;        /* Generate 'ecx' as pixels [8,9,10,11] */
		pixelspace[2] = ecx;
		LOW_BUILD_PIXELS_3 ;        /* Generate 'ecx' as pixels [0,1,2,3] */
		pixelspace[0] = ecx;
#else
		LOW_BUILD_PIXELS_0 ;        /* Generate 'ecx' as pixels [4,5,6,7] */
		pixelspace[1] = ecx;
		LOW_BUILD_PIXELS_1 ;        /* Generate 'ecx' as pixels [12,13,14,15] */
		pixelspace[3] = ecx;
		LOW_BUILD_PIXELS_2 ;        /* Generate 'ecx' as pixels [0,1,2,3] */
		pixelspace[0] = ecx;
		LOW_BUILD_PIXELS_3 ;        /* Generate 'ecx' as pixels [8,9,10,11] */
		pixelspace[2] = ecx;
#endif
		/* And plot, the Spec512 is offset by 1 pixel and works on 'chunks' of 4 pixels */
		/* So, we plot 1_4_4_4_3 to give 16 pixels, changing palette between */
		/* (last one is used for first of next 16-pixels) */
		ecx = pixelspace[0];
		PLOT_SPEC512_LEFT_LOW_640_16BIT(0);
		Spec512_UpdatePaletteSpan();

		ecx = GET_SPEC512_OFFSET_PIXELS(pixelspace, 1);
		PLOT_SPEC512_MID_640_16BIT(1);
		Spec512_UpdatePaletteSpan();

		ecx = GET_SPEC512_OFFSET_PIXELS(pixelspace, 5);
		PLOT_SPEC512_MID_640_16BIT(5);
		Spec512_UpdatePaletteSpan();

		ecx = GET_SPEC512_OFFSET_PIXELS(pixelspace, 9);
		PLOT_SPEC512_MID_640_16BIT(9);
		Spec512_UpdatePaletteSpan();

		ecx = GET_SPEC512_OFFSET_FINAL_PIXELS(pixelspace);
		PLOT_SPEC512_END_LOW_640_16BIT(13);

		esi += 16;                  /* Next PC pixels */
		edi += 2;                   /* Next ST pixels */
		ebp += 2;                   /* Next ST copy pixels */
	}
	while (--x);                    /* Loop on X */

	Spec512_EndScanLine();
}
