
uint8_t findMax(uint8_t *tag, uint8_t len)
{
	uint8_t i,imax,index;
	i = 0;
	imax = tag[0];
	index = 0;
	for(i = 1; i < len; i++)
	{
		if(tag[i] > imax)
		{
			imax = tag[i];
			index = i;
		}
	}
//	return index;
	return tag[index];
}

uint8_t findMin(uint8_t *tag, uint8_t len)
{
	uint8_t i,imin,index;
	i = 0;
	imin = tag[0];
	index = 0;
	for(i = 1; i < len; i++)
	{
		if(tag[i] < imin)
		{
			imin = tag[i];
			index = i;
		}
	}
//	return index;
	return tag[index];
}

uint8_t findMaxIndex(uint8_t *tag, uint8_t len)
{
	uint8_t i,imax,index;
	i = 0;
	imax = tag[0];
	index = 0;
	for(i = 1; i < len; i++)
	{
		if(tag[i] > imax)
		{
			imax = tag[i];
			index = i;
		}
	}
	return index;
//	return tag[index];
}

uint8_t findMinIndex(uint8_t *tag, uint8_t len)
{
	uint8_t i,imin,index;
	i = 0;
	imin = tag[0];
	index = 0;
	for(i = 1; i < len; i++)
	{
		if(tag[i] < imin)
		{
			imin = tag[i];
			index = i;
		}
	}
	return index;
//	return tag[index];
}

uint8_t findWaveVally(uint8_t *tag, uint8_t len)
{
		uint8_t half_len;
		half_len = len >> 1;
		for(uint8_t i = 0; i < half_len; i++)
		{
			if( tag[half_len] < tag[i] )
			{
				
			}
			else
				return 0;
		}
		for(uint8_t i = half_len + 1; i < len; i++)
		{
			if( tag[half_len] <= tag[i] )
			{
				
			}
			else
				return 0;
		}
		return half_len;
}

uint8_t findWavePeak(uint8_t *tag, uint8_t len)
{
		uint8_t half_len;
		half_len = len >> 1;
		for(uint8_t i = 0; i < half_len; i++)
		{
			if( tag[half_len] > tag[i] )
			{
				
			}
			else
				return 0;
		}
		for(uint8_t i = half_len + 1; i < len; i++)
		{
			if( tag[half_len] >= tag[i] )
			{
				
			}
			else
				return 0;
		}
		return half_len;
}

uint8_t hexToChar(uint8_t data)
{
//	uint8_t result = 'A';
	if(data > 9)
	{
		return (data-10+'A');
	}
	else
	{
		return (data+0x30);
	}
}

uint8_t dataAlikeCheck(uint8_t a, uint8_t b, uint8_t index)
{
	if( a < 1 || b < 1 )
		return 0;
	if( a <= index && b <= index )
	{
		return 1;
	}
	
	if( a > b )
	{
		if( b >= a -index )
			return 1;
		else
			return 0;
	}
	else
	{
		if( a >= b -index )
			return 1;
		else
			return 0;
	}
}

void stackShift(uint8_t *pdata, uint8_t shiftlen, uint8_t len)
{
	for( uint8_t i = 0; i < len; i++)
	{
		pdata[i] = pdata[i + shiftlen];
	}
}

#define VOLTAGE_LOW					50		//0.2V
#define VOLTAGE_HIGH				208		//3.0V

uint8_t wristbandIsOnHand(uint8_t state, uint16_t interval, uint8_t *pdata, uint8_t len)
{
	uint8_t lowcnt, highcnt, formerlow;//formerhigh;
//	uint8_t lowfirst;
	
//	lowfirst = 0;
	lowcnt = highcnt = formerlow = 0;
	DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "wristband state %d\n", 1, state);
	if(state)
	{
		for(uint8_t i = 0; i < len-1; i++)
		{
			if(pdata[i] < VOLTAGE_LOW)
			{
				lowcnt++;
				formerlow = lowcnt;
			}
			else if(pdata[i] > VOLTAGE_HIGH)
			{
				if(formerlow*interval > 600)//600MS
				{
					highcnt++;
					if(highcnt*interval > 1000)//1000MS
					{
						DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "bandIsOff low=%d,high=%d\n", 2, lowcnt, highcnt);
						return 0;
					}
				}
			}
			else
			{
				lowcnt = 0;
				highcnt = 0;
			}			
		}
	}
	else
	{
		for(uint8_t i = 0; i < len; i++)
		{
			if(pdata[i] > VOLTAGE_HIGH)
			{
					highcnt++;
					if(highcnt*interval > 800)//1000MS
					{
						DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "bandIsOn high= %d\n", 1, highcnt);
						return 1;
					}
			}
		}
	}
	return state;
}



uint16_t heart_rate_algorithm2(uint8_t *raw_data, uint8_t interval, uint8_t len, uint8_t *level)
{
			uint8_t sub_data[100];
			uint8_t maxs[10],mins[10];
			uint32_t avg = 0;
			uint8_t j,k;
			uint16_t slop;
			uint8_t slopRef;
//			uint16_t avg_max,avg_min;
			uint8_t temp_a, temp_b, temp_c,temp_d;
//			uint8_t index_a, index_b, index_c;
	
			avg = 0;
			for(uint8_t i = 0; i < len-ADD_NUM; i++)
			{
				sub_data[i] = ( raw_data[i+0] + raw_data[i+1] + raw_data[i+2] + raw_data[i+3] )>>2;
			}
			j = 0;
			k = 0;


			for(uint8_t i = 0; i < len-ADD_NUM-FIND_NUM; i++)
			{
					if( findWavePeak( &sub_data[i], FIND_NUM) )
					{
						maxs[j++] = i + (FIND_NUM >> 1);
						temp_a = i + (FIND_NUM >> 1);
						maxs[k] = temp_a + findMaxIndex(&raw_data[temp_a], ADD_NUM);
					}
					if(j >= 10)
						break;

					if( findWaveVally( &sub_data[i], FIND_NUM ) )
					{
			//			mins[k] = i + (FIND_NUM >> 1);
						temp_a = i + (FIND_NUM >> 1);
						mins[k] = temp_a + findMinIndex(&raw_data[temp_a], ADD_NUM);
						DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "find Vally = %d\n", 1, mins[k]);
						k++;
					}
					if(k >= 10)
						break;
			}
			
			
			
		#if 1 // three circles
			if( k < 4 )
				return 0;
		#else //two circles
			if( k < 3 )
				return 0;
		#endif
			
			slop = raw_data[maxs[0]] - raw_data[mins[0]];
			for(uint8_t i = 0; i < j && i < k ; i++)
			{
				slop = raw_data[maxs[i]] - raw_data[mins[i]];
				slop >>= 1;
			}
			slopRef = slop;
			DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "slop = %d\n", 1, slop);
			slopRef >>= 2;
			slopRef += (slop >> 3);
			DBG_BUFFER(MODULE_APP, LEVEL_ERROR, "slopRef = %d\n", 1, slopRef);
			if(slopRef < 12)
				slopRef = 12;
			
			#if 1// three circles
			for( uint8_t i = 3; i < k; i++)
			{
				temp_a = findMin(&raw_data[mins[i-3]], ADD_NUM);////raw_data[mins[0] + 3];
				temp_b = findMin(&raw_data[mins[i-2]], ADD_NUM);////raw_data[mins[0] + 3];
				temp_c = findMin(&raw_data[mins[i-1]], ADD_NUM);
				temp_d = findMin(&raw_data[mins[i]], ADD_NUM);
				if(dataAlikeCheck(temp_a, temp_b, slopRef) && dataAlikeCheck(temp_b, temp_c, slopRef) && dataAlikeCheck(temp_d, temp_c, slopRef))
				{
					if(dataAlikeCheck( mins[i] - mins[i-1], mins[i-1] - mins[i-2] , 3) && dataAlikeCheck( mins[i-2] - mins[i-3], mins[i-1] - mins[i-2] , 3))
					{
							avg = ( 60000 *3 ) /  ( ( mins[i] - mins[i-3] )*interval );
							return (uint16_t)avg;
					}
				}
			}		
			#else // two circles
			for( uint8_t i = 2; i < k; i++)
			{
				temp_a = findMin(&raw_data[mins[i-3]], ADD_NUM);////raw_data[mins[0] + 3];
				temp_b = findMin(&raw_data[mins[i-2]], ADD_NUM);////raw_data[mins[0] + 3];
				temp_c = findMin(&raw_data[mins[i-1]], ADD_NUM);
				if(dataAlikeCheck(temp_a, temp_b, slopRef) && dataAlikeCheck( temp_a, temp_c, slopRef) && dataAlikeCheck(temp_b, temp_c, slopRef) )
				{
					if(dataAlikeCheck( mins[i] - mins[i-1], mins[i-1] - mins[i-2] , 3) )
					{
							avg = ( 60000 *2 ) /  ( ( mins[i] - mins[i-2] )*interval );
							return (uint16_t)avg;
					}
				}
			}		
			#endif
			return 0;
}

