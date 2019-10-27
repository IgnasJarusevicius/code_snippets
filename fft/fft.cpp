/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   fft.cpp
 * Author: ignas
 *
 * Created on June 23, 2019, 8:59 PM
 */

#include <vector>
#include <complex>

void fft(std::vector<std::complex<float>> &x)
{
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	std::complex<float> phiT = std::complex<float>(cos(thetaT), -sin(thetaT)), T;
	while (k > 1) {
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++) {
			for (unsigned int a = l; a < N; a += n)	{
				unsigned int b = a + k;
				std::complex<float> t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++) {
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a) {
			std::complex<float> t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
}

std::vector<std::complex<float>> coef;
std::vector<uint32_t> indices;
std::vector<std::complex<float>> output;

uint32_t bitfieldReverse(uint32_t b)
{
    b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
    b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
    b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
    b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
    b = ((b >> 16) | (b << 16));
    return b;
}

void precompute_trig(int N)
{
    const double pi = acos(-1);
    uint32_t m = 32u - (uint32_t)log2(N);
    double t = 2.0 * pi / double(N);
    coef.resize(N/2);
    indices.resize(N);
    for (int i = 0; i < N/2; i++)
        coef[i] = std::complex<float>(cos(i*t), -sin(i*t));
    for (int i = 0; i < N; i++)
        indices[i] = bitfieldReverse(i)>>m;
    output.resize(N);
}

void fft_inplace(std::vector<std::complex<float>> &x)
{
	unsigned int N = x.size(), k = N, n;
	while (k > 1) {
		n = k;
		k >>= 1;
		for (unsigned int l = 0; l < k; l++) {
			for (unsigned int a = l; a < N; a += n)	{
				unsigned int b = a + k;
				std::complex<float> t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * coef[l*N/n];
			}
		}
	}
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++) {
		unsigned int b = a;
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a) {
			std::complex<float> t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
}

void fft_outplace(std::vector<std::complex<float>> &input)
{
	unsigned int N = input.size();
    uint32_t m = 0;
	for (int k = 1; k<N; k<<=1)	{
		for (unsigned int a = 0; a < N/2; a++) {
            unsigned int b = a + N/2;
            unsigned int c = (a/k)*k;
            if (m%2 == 1) {
                input[2*a] = output[a] + output[b];
                input[2*a+1] = (output[a] - output[b]) * coef[c];
            }
            else {
                output[2*a] = input[a] + input[b];
                output[2*a+1] = (input[a] - input[b]) * coef[c];
            }
		}
        m++;
	}

    if (m%2 == 0)
        for (unsigned int a = 0; a < N/2; a++) {
            output[2*a] = input[2*a];
            output[2*a+1] = input[2*a+1];
        }

	for (unsigned int a = 0; a < N/2; a++) {
        input[2*a] = output[indices[2*a]];
        input[2*a+1] = output[indices[2*a+1]];
    }
}



std::string fft_source =
        "#version 430\n"
        "layout(local_size_x=1024) in;\n"
        "layout(location = 0) uniform int N;\n"
        "layout(std430, binding=0) buffer pblock { vec2 positions[]; };\n"
        "layout(std430, binding=1) buffer cblock { vec2 coef[]; };\n"
        "layout(std430, binding=2) buffer oblock { vec2 tmp[]; };\n"
        "layout(std430, binding=3) buffer iblock { uint indices[]; };\n"

        "vec2 mult_comlex(in vec2 a, in vec2 b) {\n"
        "    return mat2(a, -a.y, a.x)*b;\n"
        "}"

        "void main() {\n"
        "uint a, c, m=0;\n"
        "uint start = gl_GlobalInvocationID.x*N/gl_WorkGroupSize.x;\n"
        "uint stop = (gl_GlobalInvocationID.x+1).x*N/gl_WorkGroupSize.x;\n"
        "uvec3 pos;\n"
        "for (uint k = 1; k<N; k<<=1) {\n"
        "   pos = uvec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.x+N/2, 2*gl_GlobalInvocationID.x);\n"
        "   do{\n"
        "       c = (pos.x/k)*k;\n"
        "       if ((m&1u) == 1){\n"
        "           positions[pos.z] = tmp[pos.x] + tmp[pos.y];\n"
        "           positions[pos.z+1] = mult_comlex(tmp[pos.x] - tmp[pos.y], coef[c]);\n"
        "           }else{\n"
        "           tmp[pos.z] = positions[pos.x] + positions[pos.y];\n"
        "           tmp[pos.z+1] = mult_comlex(positions[pos.x] - positions[pos.y], coef[c]);\n"
        "           }\n"
        "       pos += gl_NumWorkGroups*uvec3(gl_WorkGroupSize.x, gl_WorkGroupSize.x, 2*gl_WorkGroupSize.x);\n"
        "       } while (pos.y < N);\n"
        "   m++;\n"
        "   barrier();\n"
        "   }\n"
        "for (a = start; a != stop; a++)\n"
        "   tmp[a] = ((m+1)&1)*positions[a]+(m&1)*tmp[a];\n"
        "barrier();\n"
        "for (a = start; a != stop; a++)\n"
        "   positions[a].x = length(tmp[indices[a]])/N;\n"
        "}\n";


std::string fft_source_slow =
        "#version 430\n"
        "layout(local_size_x=1) in;\n"
        "layout(location = 0) uniform int N;\n"
        "layout(std430, binding=0) buffer pblock { vec2 positions[]; };\n"
        "layout(std430, binding=1) buffer cblock { vec2 coef[]; };\n"

        "void main() {\n"
        "int k = N;\n"
        "while (k > 1) {\n"
        "   int n = k;\n"
        "   k >>= 1;\n"
        "   for (int l = 0; l < k; l++) {\n"
        "       for (int a = l; a < N; a += n){\n"
        "           int b = a + k;\n"
        "           vec2 t = positions[a] - positions[b];\n"
        "           positions[a] += positions[b];\n"
        "           vec2 c = coef[l*N/n];\n"
        "           positions[b] = vec2(t.x * c.x - t.y * c.y, t.y * c.x + t.x * c.y);\n"
        "           }\n"
        "       }\n"
        "   }\n"
        "uint m = uint(log2(N)+0.1);\n"
        "for (uint a = 0; a < N; a++){\n"
        "   uint b = bitfieldReverse(a);\n"
        "   b = b >> (32-m);\n"
        "   if (b > a){\n"
        "       vec2 t = positions[a];\n"
        "       positions[a] = -positions[b];\n"
        "       positions[b] = t;\n"
        "       }\n"
        "   }\n"
       "for (int a = 0; a < N; a++)\n"
        "   positions[a].x = length(positions[a])/N;\n"
        "}\n";

std::string fft_ok_source =
        "#version 430\n"
        "layout(local_size_x=1024) in;\n"
        "layout(location = 0) uniform int N;\n"
        "layout(std430, binding=0) buffer pblock { vec2 positions[]; };\n"
        "layout(std430, binding=1) buffer cblock { vec2 coef[]; };\n"
        "layout(std430, binding=2) buffer oblock { vec2 tmp[]; };\n"
        "layout(std430, binding=3) buffer iblock { uint indices[]; };\n"

        "vec2 mult_comlex(in vec2 a, in vec2 b) {\n"
        "    return vec2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);\n"
        "}"

        "void main() {\n"
        "uint m = 0;\n"
        "for (uint k = 1; k<N; k<<=1) {\n"
        "   for (uint a = gl_LocalInvocationID.x; a < N/2; a+=gl_WorkGroupSize.x){\n"
        "       uint b = a + N/2;\n"
        "       uint c = (a/k)*k;\n"
        "       if (m%2 == 1){\n"
        "           positions[2*a] = tmp[a] + tmp[b];\n"
        "           positions[2*a+1] = mult_comlex(tmp[a] - tmp[b], coef[c]);\n"
        "           }else{\n"
        "           tmp[2*a] = positions[a] + positions[b];\n"
        "           tmp[2*a+1] = mult_comlex(positions[a] - positions[b], coef[c]);\n"
        "           }\n"
        "       }\n"
        "   m++;\n"
        "   barrier();\n"
        "   }\n"
        "if (m%2 == 0){\n"
        "   for (uint a = gl_LocalInvocationID.x; a < N; a+=gl_WorkGroupSize.x)\n"
        "       tmp[a] = positions[a];\n"
        "   barrier();}\n"
        "for (uint a = gl_LocalInvocationID.x; a < N; a+=gl_WorkGroupSize.x)\n"
        "   positions[a].x = length(tmp[indices[a]])/N;\n"
        "}\n";

std::string test_source =
        "#version 430\n"
        "layout(local_size_x=1) in;\n"
        "layout(location = 0) uniform int N;\n"
        "layout(std430, binding=0) buffer pblock { vec2 positions[]; };\n"
        "layout(std430, binding=1) buffer cblock { vec2 coef[]; };\n"

        "void main() {\n"
        "for (int a = 0; a < N; a++)\n"
        "   positions[a].x = positions[a].y;\n"
        "}\n";

