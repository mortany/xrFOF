#ifndef	SHADOW_H
#define SHADOW_H

#include "common.h"

//uniform	sampler	s_smap	: register(ps,s0);	// 2D/cube shadowmap
//Texture2D<float>	s_smap;		// 2D/cube shadowmap
//	Used for RGBA texture too ?!
Texture2D	s_smap : register(ps,t0);		// 2D/cube shadowmap

Texture2D<float>	s_smap_minmax;		// 2D/cube shadowmap
#include "gather.ps"

SamplerComparisonState		smp_smap;	//	Special comare sampler
sampler		smp_jitter;

Texture2D	jitter0;
Texture2D	jitter1;
//uniform sampler2D	jitter2;
//uniform sampler2D	jitter3;
//uniform float4 		jitterS;

Texture2D	jitterMipped;

#ifndef USE_ULTRA_SHADOWS
#define	KERNEL	0.6f
#else
#define	KERNEL	1.0f
#endif

float modify_light( float light )
{
   return ( light > 0.7 ? 1.0 : lerp( 0.0, 1.0, saturate( light / 0.7 ) ) ); 
}

//////////////////////////////////////////////////////////////////////////////////////////
// hardware + PCF
//////////////////////////////////////////////////////////////////////////////////////////
float sample_hw_pcf (float4 tc,float4 shift)
{
	static const float 	ts = KERNEL / float(SMAP_size);

	tc.xyz 	/= tc.w;
	tc.xy 	+= shift.xy * ts;

	return s_smap.SampleCmpLevelZero( smp_smap, tc.xy, tc.z).x;
}

#define GS2 3

float shadow_hw( float4 tc )
{
  	float	s0		= sample_hw_pcf( tc, float4( -1, -1, 0, 0) );
  	float	s1		= sample_hw_pcf( tc, float4( +1, -1, 0, 0) );
  	float	s2		= sample_hw_pcf( tc, float4( -1, +1, 0, 0) );
  	float	s3		= sample_hw_pcf( tc, float4( +1, +1, 0, 0) );

	return	(s0+s1+s2+s3)/4.h;
}

#if SUN_QUALITY>=4
#define FILTER_SIZE	11
#define FS  FILTER_SIZE
#define FS2 ( FILTER_SIZE / 2 )

static const float W2[11][11] = 
                 { { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 }, 
			       { 0.0,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.0 },
			       { 0.0,0.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.2,0.0 },
			       { 0.0,0.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.2,0.0 },
			       { 0.0,0.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.2,0.0 },
			       { 0.0,0.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.2,0.0 },
			       { 0.0,0.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.2,0.0 },
			       { 0.0,0.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.2,0.0 },
			       { 0.0,0.2,1.0,1.0,1.0,1.0,1.0,1.0,1.0,0.2,0.0 },
			       { 0.0,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       };

static const float W1[11][11] = 
                 { { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 }, 
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.0,0.0 },
			       { 0.0,0.0,0.2,1.0,1.0,1.0,1.0,1.0,0.2,0.0,0.0 },
			       { 0.0,0.0,0.2,1.0,1.0,1.0,1.0,1.0,0.2,0.0,0.0 },
			       { 0.0,0.0,0.2,1.0,1.0,1.0,1.0,1.0,0.2,0.0,0.0 },
			       { 0.0,0.0,0.2,1.0,1.0,1.0,1.0,1.0,0.2,0.0,0.0 },
			       { 0.0,0.0,0.2,1.0,1.0,1.0,1.0,1.0,0.2,0.0,0.0 },
			       { 0.0,0.0,0.2,0.2,0.2,0.2,0.2,0.2,0.2,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       };

static const float W0[11][11] = 
                 { { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 }, 
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.1,0.1,0.1,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.1,1.0,0.1,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.1,0.1,0.1,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       { 0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0 },
			       };

float Fw( int r, int c, float fL )
{
	return        (1.0-fL) * (1.0-fL) * (1.0-fL) * W0[r][c] +
	       3.0f * (1.0-fL) * (1.0-fL) *   fL     * W1[r][c] +
	       3.0f *    fL    *    fL    * (1.0-fL) * W2[r][c] +
                     fL    *    fL    *   fL     * 1.0f;
} 

#define BLOCKER_FILTER_SIZE	11
#define BFS  BLOCKER_FILTER_SIZE
#define BFS2 ( BLOCKER_FILTER_SIZE / 2 )

#define SUN_WIDTH 300.0f

// uses gather for DX11/10.1 and visibilty encoding for DX10.0
static const float2 poissonDisk[16] = {
        float2( -0.94201624f, -0.39906216f ),
        float2( 0.94558609f, -0.76890725f ),
        float2( -0.094184101f, -0.92938870f ),
        float2( 0.34495938f, 0.29387760f ),
        float2( -0.91588581f, 0.45771432f ),
        float2( -0.81544232f, -0.87912464f ),
        float2( -0.38277543f, 0.27676845f ),
        float2( 0.97484398f, 0.75648379f ),
        float2( 0.44323325f, -0.97511554f ),
        float2( 0.53742981f, -0.47373420f ),
        float2( -0.26496911f, -0.41893023f ),
        float2( 0.79197514f, 0.19090188f ),
        float2( -0.24188840f, 0.99706507f ),
        float2( -0.81409955f, 0.91437590f ),
        float2( 0.19984126f, 0.78641367f ),
        float2( 0.14383161f, -0.14100790f )
    };

#define PCSS_PIXEL             5
#define PCSS_STEP             5
#define PCSS_PIXEL_MIN         1.0f
#define PCSS_SUN_WIDTH     300.f
#define PCSS_NUM_SAMPLES     16

float shadow_extreme_quality( float3 tc )
{
    int3 uv                     = int3(tc.xy * float(SMAP_size), 0);
    float     zBlock                = tc.z - 0.0001f;
    float    avgBlockerDepth     = 0.f;
    float    blockerCount         = 0.f;

    [unroll] for( int row = -PCSS_PIXEL; row <= PCSS_PIXEL; row += PCSS_STEP )
    {
        [unroll] for( int col = -PCSS_PIXEL; col <= PCSS_PIXEL; col += PCSS_STEP )
        {
            float shadowMapDepth = s_smap.Load( uv, int2( col, row ) ).x;
            float b1 = ( shadowMapDepth < zBlock ) ? 1.f : 0.f;
            blockerCount += b1;
            avgBlockerDepth += shadowMapDepth * b1;
        }
    }

    if( blockerCount < 1 || blockerCount >= 9 )
        return 1.f - min(1.f, blockerCount);

    avgBlockerDepth /= blockerCount;
    float fRatio = saturate( ( ( tc.z - avgBlockerDepth ) * PCSS_SUN_WIDTH ) / avgBlockerDepth );
    fRatio *= fRatio;
    fRatio = max(PCSS_PIXEL_MIN, fRatio * float(PCSS_PIXEL)) / float(SMAP_size);
    
    float s = 0.f;
    [unroll] for( uint i = 0; i < PCSS_NUM_SAMPLES; ++i )
    {
        float2 offset = poissonDisk[i] * fRatio;
        s += s_smap.SampleCmpLevelZero( smp_smap, tc.xy + offset, tc.z ).x;
    }
    return s / PCSS_NUM_SAMPLES;
}

float4 Fw( int r, int c )
{
	return float4( W0[r][c], W1[r][c], W2[r][c], 1.0f );
}

//======================================================================================
// This shader computes the contact hardening shadow filter
//======================================================================================
float shadow_extreme_quality_fused( float3 tc )
{
    float4 s   = (0.0f).xxxx;
    float2 stc = ( SMAP_size * tc.xy ) + float2( 0.5, 0.5 );
    float2 tcs = floor( stc );
    float2 fc;
    int    row;
    int    col;
    float  w = 0.0;
    float  avgBlockerDepth = 0;
    float  blockerCount = 0;
    float  fRatio;
    float4 v1[ FS2 + 1 ];
    float2 v0[ FS2 + 1 ];
    float2 off;

    fc     = stc - tcs;
    tc.xy  = tc.xy - ( fc * (1.0f/SMAP_size) );

    // filter shadow map samples using the dynamic weights
    [unroll(FS)]for( row = -FS2; row <= FS2; row += 2 )
    {
        for( col = -FS2; col <= FS2; col += 2 )
        {
            float4 d4;
            
#ifndef PS_4            
            d4 = s_smap.Gather( smp_nofilter, tc.xy + (1.0f/SMAP_size) * float2( col, row ) );
#else
			d4.w = s_smap.SampleLevel( smp_nofilter, tc.xy + (1.0f/SMAP_size) * float2( col, row ), 0 ).x;
			d4.z = s_smap.SampleLevel( smp_nofilter, tc.xy + (1.0f/SMAP_size) * float2( col+1, row ) , 0 ).x;
			d4.y = s_smap.SampleLevel( smp_nofilter, tc.xy + (1.0f/SMAP_size) * float2( col+1, row+1 ), 0 ).x;
			d4.x = s_smap.SampleLevel( smp_nofilter, tc.xy + (1.0f/SMAP_size) * float2( col, row+1 ), 0 ).x;
#endif
            float4 b4  = ( tc.zzzz <= d4 ) ? (0.0f).xxxx : (1.0f).xxxx;   

            v1[(col+FS2)/2] = ( tc.zzzz <= d4 ) ? (1.0f).xxxx : (0.0f).xxxx;
            blockerCount += dot( b4, (1.0).xxxx );
            avgBlockerDepth += dot( d4, b4 );
          
            if( col == -FS2 )
            {
                s += ( 1 - fc.y ) * ( v1[0].w * ( Fw(row+FS2,0) - 
                                      Fw(row+FS2,0) * fc.x ) + v1[0].z * 
                                    ( fc.x * ( Fw(row+FS2,0) - 
                                      Fw(row+FS2,1) ) +  
                                      Fw(row+FS2,1) ) );
                s += (     fc.y ) * ( v1[0].x * ( Fw(row+FS2,0) - 
                                      Fw(row+FS2,0) * fc.x ) + 
                                      v1[0].y * ( fc.x * ( Fw(row+FS2,0) - 
                                      Fw(row+FS2,1) ) +  
                                      Fw(row+FS2,1) ) );
                if( row > -FS2 )
                {
                    s += ( 1 - fc.y ) * ( v0[0].x * ( Fw(row+FS2-1,0) - 
                                          Fw(row+FS2-1,0) * fc.x ) + v0[0].y * 
                                        ( fc.x * ( Fw(row+FS2-1,0) - 
                                          Fw(row+FS2-1,1) ) +  
                                          Fw(row+FS2-1,1) ) );
                    s += (     fc.y ) * ( v1[0].w * ( Fw(row+FS2-1,0) - 
                                          Fw(row+FS2-1,0) * fc.x ) + v1[0].z * 
                                        ( fc.x * ( Fw(row+FS2-1,0) - 
                                          Fw(row+FS2-1,1) ) +  
                                          Fw(row+FS2-1,1) ) );
                }
            }
            else if( col == FS2 )
            {
                s += ( 1 - fc.y ) * ( v1[FS2].w * ( fc.x * ( Fw(row+FS2,FS-2) - 
                                      Fw(row+FS2,FS-1) ) + 
                                      Fw(row+FS2,FS-1) ) + v1[FS2].z * fc.x * 
                                      Fw(row+FS2,FS-1) );
                s += (     fc.y ) * ( v1[FS2].x * ( fc.x * ( Fw(row+FS2,FS-2) - 
                                      Fw(row+FS2,FS-1) ) + 
                                      Fw(row+FS2,FS-1) ) + v1[FS2].y * fc.x * 
                                      Fw(row+FS2,FS-1) );
                if( row > -FS2 )
                {
                    s += ( 1 - fc.y ) * ( v0[FS2].x * ( fc.x * 
                                        ( Fw(row+FS2-1,FS-2) - 
                                          Fw(row+FS2-1,FS-1) ) + 
                                          Fw(row+FS2-1,FS-1) ) + 
                                          v0[FS2].y * fc.x * Fw(row+FS2-1,FS-1) );
                    s += (     fc.y ) * ( v1[FS2].w * ( fc.x * 
                                        ( Fw(row+FS2-1,FS-2) - 
                                          Fw(row+FS2-1,FS-1) ) + 
                                          Fw(row+FS2-1,FS-1) ) + 
                                          v1[FS2].z * fc.x * Fw(row+FS2-1,FS-1) );
                }
            }
            else
            {
                s += ( 1 - fc.y ) * ( v1[(col+FS2)/2].w * ( fc.x * 
                                    ( Fw(row+FS2,col+FS2-1) - 
                                      Fw(row+FS2,col+FS2+0) ) + 
                                      Fw(row+FS2,col+FS2+0) ) +
                                      v1[(col+FS2)/2].z * ( fc.x * 
                                    ( Fw(row+FS2,col+FS2-0) - 
                                      Fw(row+FS2,col+FS2+1) ) + 
                                      Fw(row+FS2,col+FS2+1) ) );
                s += (     fc.y ) * ( v1[(col+FS2)/2].x * ( fc.x * 
                                    ( Fw(row+FS2,col+FS2-1) - 
                                      Fw(row+FS2,col+FS2+0) ) + 
                                      Fw(row+FS2,col+FS2+0) ) +
                                      v1[(col+FS2)/2].y * ( fc.x * 
                                    ( Fw(row+FS2,col+FS2-0) - 
                                      Fw(row+FS2,col+FS2+1) ) + 
                                      Fw(row+FS2,col+FS2+1) ) );
                if( row > -FS2 )
                {
                    s += ( 1 - fc.y ) * ( v0[(col+FS2)/2].x * ( fc.x * 
                                        ( Fw(row+FS2-1,col+FS2-1) - 
                                          Fw(row+FS2-1,col+FS2+0) ) + 
                                          Fw(row+FS2-1,col+FS2+0) ) +
                                          v0[(col+FS2)/2].y * ( fc.x * 
                                        ( Fw(row+FS2-1,col+FS2-0) - 
                                          Fw(row+FS2-1,col+FS2+1) ) + 
                                          Fw(row+FS2-1,col+FS2+1) ) );
                    s += (     fc.y ) * ( v1[(col+FS2)/2].w * ( fc.x * 
                                        ( Fw(row+FS2-1,col+FS2-1) - 
                                          Fw(row+FS2-1,col+FS2+0) ) + 
                                          Fw(row+FS2-1,col+FS2+0) ) +
                                          v1[(col+FS2)/2].z * ( fc.x * 
                                        ( Fw(row+FS2-1,col+FS2-0) - 
                                          Fw(row+FS2-1,col+FS2+1) ) + 
                                          Fw(row+FS2-1,col+FS2+1) ) );
                }
            }
            
            if( row != FS2 )
            {
                v0[(col+FS2)/2] = v1[(col+FS2)/2].xy;
            }
        }
    }

    // compute ratio using formulas from PCSS
    if( blockerCount > 0.0 )
    {
        avgBlockerDepth /= blockerCount;
        fRatio = saturate( ( ( tc.z - avgBlockerDepth ) * SUN_WIDTH ) / avgBlockerDepth );
        fRatio *= fRatio;
    }
    else
    {
        fRatio = 0.0; 
    }

    // sum up weights of dynamic filter matrix
    for( row = 0; row < FS; ++row )
    {
       for( col = 0; col < FS; ++col )
       {
          w += Fw(row,col,fRatio);
       }
    }

    return dot(s, float4((1.0f-fRatio)*(1.0f-fRatio)*(1.0f-fRatio),
						 3.0f * (1.0-fRatio)*(1.0-fRatio)*fRatio,
						 3.0f * fRatio*fRatio*(1.0-fRatio),
						 fRatio*fRatio*fRatio ) )/w;
}
#endif

#ifdef SM_4_1

float dx10_1_hw_hq_7x7( float3 tc )
{
   float  s = 0.0f;
   float2 stc = ( SMAP_size * tc.xy ) + float2( 0.5, 0.5 );
   float2 tcs = floor( stc );
   float2 fc;
   int    row;
   int    col;

   fc.xy = stc - tcs;
   tc.xy = tcs * ( 1.0 / SMAP_size );
   
   // loop over the rows
   for( row = -GS2; row <= GS2; row += 2 )
   {
       [unroll]for( col = -GS2; col <= GS2; col += 2 )
       {
            float4 v = ( tc.zzzz <= s_smap.Gather( smp_nofilter, tc.xy, int2( col, row ) ) ) ? (1.0).xxxx : (0.0).xxxx; 
            
            if( row == -GS2 ) // top row
            {
                if( col == -GS2 ) // left
                    s += dot( float4( 1.0-fc.x, 1.0, 1.0-fc.y, (1.0-fc.x)*(1.0-fc.y) ), v );
                else if( col == GS2 ) // right
                    s += dot( float4( 1.0f, fc.x, fc.x*(1.0-fc.y), 1.0-fc.y ), v );
                else // center
                    s += dot( float4( 1.0, 1.0, 1.0-fc.y, 1.0-fc.y ), v );
            }
            else if( row == GS2 )  // bottom row
            {
                if( col == -GS2 ) // left
                    s += dot( float4( (1.0-fc.x)*fc.y, fc.y, 1.0, (1.0-fc.x) ), v );
                else if( col == GS2 ) // right
                    s += dot( float4( fc.y, fc.x*fc.y, fc.x, 1.0 ), v );
                else // center
                    s += dot( float4(fc.yy,1.0,1.0), v );
            }
            else // center rows
            {
                if( col == -GS2 ) // left
                    s += dot( float4( (1.0-fc.x), 1.0, 1.0, (1.0-fc.x) ), v ); 
                else if( col == GS2 ) // right
                    s += dot( float4( 1.0, fc.x, fc.x, 1.0 ), v ); 
                else // center
                    s += dot( (1.0).xxxx, v ); 
            }
        }
   }
  
   return s*(1.0/49.0);
}

#endif

float dx10_0_hw_hq_7x7( float4 tc )
{
   tc.xyz /= tc.w;

   float  s   = 0.0;
   float2 stc = ( SMAP_size * tc.xy ) + float2( 0.5, 0.5 );
   float2 tcs = floor( stc );
   float2 fc;

   fc    = stc - tcs;
   tc.xy = tc.xy - ( fc * ( 1.0/SMAP_size ) );

   float2 pwAB = ( ( 2.0 ).xx - fc ); 	
   float2 tcAB = ( 1.0/SMAP_size ).xx / pwAB;
   float2 tcM  = (0.5/SMAP_size ).xx;
   float2 pwGH = ( ( 1.0 ).xx + fc );
   float2 tcGH = (1.0/SMAP_size) * ( fc / pwGH );

   for( int row = -GS2; row <= GS2; row += 2 )
   {
      for( int col = -GS2; col <= GS2; col += 2 )
	  {
		if( row == -GS2 ) // top row
		{
			if( col == -GS2 ) // left
				s += ( pwAB.x * pwAB.y ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + tcAB, tc.z, int2( col, row ) ).x;
			else if( col == GS2 ) // right
				s += ( pwGH.x * pwAB.y ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + float2( tcGH.x, tcAB.y), tc.z, int2( col, row ) ).x;
			else // center
				s += (    2.0 * pwAB.y ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + float2( tcM.x, tcAB.y), tc.z, int2( col, row ) ).x;
		}
		else if( row == GS2 )  // bottom row
		{
			if( col == -GS2 ) // left
				s += ( pwAB.x * pwGH.y ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + float2( tcAB.x, tcGH.y ), tc.z, int2( col, row ) ).x;
			else if( col == GS2 ) // right
				s += ( pwGH.x * pwGH.y ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + tcGH, tc.z, int2( col, row ) ).x;
			else // center
				s += (    2.0 * pwGH.y ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + float2( tcM.x, tcGH.y ), tc.z, int2( col, row ) ).x;
		}
		else // center rows
		{
			if( col == -GS2 ) // left
				s += ( pwAB.x * 2.0    ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + float2( tcAB.x, tcM.y ), tc.z, int2( col, row ) ).x;
			else if( col == GS2 ) // right
				s += ( pwGH.x * 2.0    ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + float2( tcGH.x, tcM.y),  tc.z, int2( col, row ) ).x;
			else // center
				s += (    2.0 * 2.0    ) * s_smap.SampleCmpLevelZero( smp_smap, tc.xy + tcM, tc.z, int2( col, row ) ).x;
		}
      }
	}

    return s/49.0;
}

#ifdef SM_MINMAX
bool cheap_reject( float3 tc, inout bool full_light ) 
{
   float4 plane0  = sm_minmax_gather( tc.xy, int2( -1,-1 ) );
   float4 plane1  = sm_minmax_gather( tc.xy, int2(  1,-1 ) );
   float4 plane2  = sm_minmax_gather( tc.xy, int2( -1, 1 ) );
   float4 plane3  = sm_minmax_gather( tc.xy, int2(  1, 1 ) );
   bool plane     = all( ( plane0 >= (0).xxxx ) * ( plane1 >= (0).xxxx ) * ( plane2 >= (0).xxxx ) * ( plane3 >= (0).xxxx ) );

   [flatten] if( !plane ) // if there are no proper plane equations in the support region
   {
      bool no_plane  = all( ( plane0 < (0).xxxx ) * ( plane1 < (0).xxxx ) * ( plane2 < (0).xxxx ) * ( plane3 < (0).xxxx ) );
      float4 z       = ( tc.z - 0.0005 ).xxxx;
      bool reject    = all( ( z > -plane0 ) * ( z > -plane1 ) * ( z > -plane2 ) * ( z > -plane3 ) ); 
      [flatten] if( no_plane && reject )
      {
         full_light = false;
         return true;
      }
      else
      {
         return false;
      }
   }
   else // plane equation detected
   {
      // compute corrected z for texel pos
      static const float scale = float( SMAP_size / 4 );
      float2 fc  = frac( tc.xy * scale );
      float  z   = lerp( lerp( plane0.y, plane1.x, fc.x ), lerp( plane2.z, plane3.w, fc.x ), fc.y );

      // do minmax test with new z
      full_light = ( ( tc.z - 0.0001 ) <= z );

      return true; 
   }
}

#endif	//	SM_MINMAX

float shadow_hw_hq( float4 tc )
{
#ifdef SM_MINMAX
   bool   full_light = false; 
   bool   cheap_path = cheap_reject( tc.xyz / tc.w, full_light );

   [branch] if( cheap_path )
   {
      [branch] if( full_light == true )
         return 1.0;
      else
         return sample_hw_pcf( tc, (0).xxxx ); 
   }
   else
   {
#if SUN_QUALITY>=4 // extreme quality
      return shadow_extreme_quality( tc.xyz / tc.w );
#else // SUN_QUALITY<4
#ifdef SM_4_1
      return dx10_1_hw_hq_7x7( tc.xyz / tc.w );
#else // SM_4_1
      return dx10_0_hw_hq_7x7( tc ); 
#endif // SM_4_1
#endif //SUN_QUALITY==4
   }
#else //	SM_MINMAX
#if SUN_QUALITY>=4 // extreme quality
      return shadow_extreme_quality( tc.xyz / tc.w );
#else // SUN_QUALITY<4
#ifdef SM_4_1
      return dx10_1_hw_hq_7x7( tc.xyz / tc.w );
#else // SM_4_1
      return dx10_0_hw_hq_7x7( tc ); 
#endif // SM_4_1
#endif //SUN_QUALITY==4
#endif //	SM_MINMAX
}

//////////////////////////////////////////////////////////////////////////////////////////
//	D24X8+PCF
//////////////////////////////////////////////////////////////////////////////////////////

float4 	test 		(float4 tc, float2 offset)
{

//	float4	tcx	= float4 (tc.xy + tc.w*offset, tc.zw);
//	return 	tex2Dproj (s_smap,tcx);

	tc.xyz 	/= tc.w;
	tc.xy 	+= offset;
	return s_smap.SampleCmpLevelZero( smp_smap, tc.xy, tc.z).x;
}

/*half 	shadowtest_sun 	(float4 tc, float4 tcJ)			// jittered sampling
{
	half4	r;

	const 	float 	scale 	= (0.5f/float(SMAP_size));

	float  	texsize = 2*SMAP_size;
	float2 	tc_J	= tc.xy/tc.w*texsize/8.0f;
	float2 	fr 		= frac(tc_J)*.5f;
	
//	half4	J0 	= tex2D	(jitter0,fr)*scale;
//	half4	J1 	= tex2D	(jitter1,fr)*scale*2;
	float4	J0 	= jitter0.Sample( smp_jitter, fr )*scale;
//	float4	J1 	= jitter1.Sample( smp_jitter, fr )*scale;

	float k = 0.99f/float(SMAP_size);
	r.x 	= test 	(tc,J0.xy+float2(-k,-k)).x;
	r.y 	= test 	(tc,J0.wz+float2( k,-k)).y;
	
 	r.z		= test	(tc,J0.xy+float2(-k, k)).z;
 	r.w		= test	(tc,J0.wz+float2( k, k)).x;
	
	half4	f;
	float k1 = 1.5f/float(SMAP_size);
	f.x 	= test 	(tc,-J0.xy+float2(-k1,0)).x;
	f.y 	= test 	(tc,-J0.wz+float2( 0,-k1)).y;

	f.z		= test	(tc,-J0.xy+float2( k1, 0)).z;
 	f.w		= test	(tc,-J0.wz+float2( 0, k1)).x;

	half res = ( r.x + r.y + r.z + r.w + f.x + f.y + f.z + f.w )*1.h/(4.h + 4.h );
	return res;
}*/
half 	shadowtest_sun 	(float4 tc, float4 tcJ)			// jittered sampling
{
	half4	r;

	//	const 	float 	scale 	= (2.0f/float(SMAP_size));
	const 	float 	scale 	= (0.7f/float(SMAP_size));


	float2 	tc_J	= frac(tc.xy/tc.w*SMAP_size/4.0f )*.5f;
	float4	J0		= jitter0.Sample(smp_jitter,tc_J)*scale;
	//half4	J1 	= tex2D	(jitter1,tc_J)*scale;

	const float k = .5f/float(SMAP_size);
	r.x 	= test 	(tc, J0.xy+half2(-k,-k)).x;
	r.y 	= test 	(tc, J0.wz+half2( k,-k)).y;
	r.z		= test	(tc,-J0.xy+half2(-k, k)).z;
	r.w		= test	(tc,-J0.wz+half2( k, k)).x;

	return	dot(r,1.h/4.h);
}

half 	shadow_high 	(float4 tc)			// jittered sampling
{

	const	float 	scale 	= (0.5f/float(SMAP_size));

	float2 	tc_J	= frac(tc.xy/tc.w*SMAP_size/4.0f )*.5f;
	float4	J0 		=	jitter0.Sample	(smp_jitter,tc_J)*scale;

	const float k = 1.f/float(SMAP_size);
	half4	r;
	r.x 	= test 	(tc,J0.xy+half2(-k,-k)).x;
	r.y 	= test 	(tc,J0.wz+half2( k,-k)).y;

	r.z		= test	(tc,J0.xy+half2(-k, k)).z;
	r.w		= test	(tc,J0.wz+half2( k, k)).x;


	const float k1 = 1.3f/float(SMAP_size);
	half4	r1;
	r1.x 	= test 	(tc,-J0.xy+half2(-k1,0)).x;
	r1.y 	= test 	(tc,-J0.wz+half2( 0,-k1)).y;

	r1.z	= test	(tc,-2*J0.xy+half2( k1, 0)).z;
	r1.w	= test	(tc,-2*J0.wz+half2( 0, k1)).x;

	return ( r.x + r.y + r.z + r.w + r1.x + r1.y + r1.z + r1.w )*1.h/8.h;
}

float shadow( float4 tc ) 
{
#ifdef USE_ULTRA_SHADOWS
#	ifdef SM_MINMAX
		return modify_light( shadow_hw_hq( tc ) ); 
#	else
		return shadow_hw_hq( tc ); 
#	endif
#else
#	if SUN_QUALITY>=2 // Hight quality
		//return shadowtest_sun 	( tc, float4(0,0,0,0) );			// jittered sampling;
		return shadow_hw		(tc);
#	else
		return shadow_hw		(tc);
#	endif
#endif
}

float shadow_volumetric( float4 tc ) 
{
	return sample_hw_pcf	(tc,float4(-1,-1,0,0)); 
}


#ifdef SM_MINMAX

//////////////////////////////////////////////////////////////////////////////////////////
// hardware + PCF
//////////////////////////////////////////////////////////////////////////////////////////

float shadow_dx10_1( float4 tc, float2 tcJ, float2 pos2d ) 
{
   return shadow( tc ); 
}

float shadow_dx10_1_sunshafts( float4 tc, float2 pos2d ) 
{
   float3 t         = tc.xyz / tc.w;
   float minmax     = s_smap_minmax.SampleLevel( smp_nofilter, t, 0 ).x;
   bool   umbra     = ( ( minmax.x < 0 ) && ( t.z > -minmax.x ) );

   [branch] if( umbra )
   {
      return 0.0;
   }
   else
   {
      return shadow_hw( tc ); 
   }
}

#endif


//////////////////////////////////////////////////////////////////////////////////////////
// testbed

//uniform sampler2D	jitter0;
//uniform sampler2D	jitter1;
float 	shadowtest 	(float4 tc, float4 tcJ)				// jittered sampling
{
	float4	r;

	const 	float 	scale 	= (2.7f/float(SMAP_size));

//	float4	J0 	= tex2Dproj	(jitter0,tcJ)*scale;
//	float4	J1 	= tex2Dproj	(jitter1,tcJ)*scale;
	tcJ.xy		/=	tcJ.w;
	float4	J0 	= jitter0.Sample( smp_jitter, tcJ )*scale;
	float4	J1 	= jitter1.Sample( smp_jitter, tcJ )*scale;

		r.x 	= test 	(tc,J0.xy).x;
		r.y 	= test 	(tc,J0.wz).y;
		r.z		= test	(tc,J1.xy).z;
		r.w		= test	(tc,J1.wz).x;

	return	dot(r,1.h/4.h);
}

float 	shadow_rain 	(float4 tc, float2 tcJ)			// jittered sampling
{
	float4	r;

	const 	float 	scale 	= (4.0f/float(SMAP_size));
//	float4	J0 	= jitter0.Sample( smp_jitter, tcJ )*scale;
//	float4	J1 	= jitter1.Sample( smp_jitter, tcJ )*scale;
	float4	J0 	= jitter0.Sample( smp_linear, tcJ )*scale;
	float4	J1 	= jitter1.Sample( smp_linear, tcJ )*scale;

	r.x 	= test 	(tc,J0.xy).x;
	r.y 	= test 	(tc,J0.wz).y;
	r.z		= test	(tc,J1.xy).z;
	r.w		= test	(tc,J1.wz).x;

//	float4	J0 	= jitterMipped.Sample( smp_base, tcJ )*scale;

//	r.x 	= test 	(tc,J0.xy).x;
//	r.y 	= test 	(tc,J0.wz).y;
//	r.z		= test	(tc,J0.yz).z;
//	r.w		= test	(tc,J0.xw).x;

	return	dot(r,1.h/4.h);
}

//////////////////////////////////////////////////////////////////////////////////////////
#ifdef  USE_SUNMASK	
float3x4 m_sunmask;	// ortho-projection
float sunmask( float4 P )
{
	float2 		tc	= mul( m_sunmask, P );		//
//	return 		tex2D( s_lmap, tc ).w;			// A8 
	return 		s_lmap.Sample( smp_linear, tc ).w;	// A8 	
}
#else
float sunmask( float4 P ) { return 1.h; }		// 
#endif
//////////////////////////////////////////////////////////////////////////////////////////
uniform float4x4	m_shadow;

#endif