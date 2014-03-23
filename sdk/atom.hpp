#pragma once


static long atom_make( long* ids )
{
	while( true )
	{
		long val = InterlockedIncrement( ids );

		if( val != 0 )
		{
			return val;
		}
	}
}

