/*****************************************************************************
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   See NOTICE file for details.
 *****************************************************************************/
#include "jpype.h"

namespace
{

template <class T>
class Convert
{
public:

	static jvalue toZ(void* c)
	{
		jvalue v;
		v.z = (*(T*) c) != 0;
		return v;
	}

	static jvalue toB(void* c)
	{
		jvalue v;
		v.b = (jbyte) (*(T*) c);
		return v;
	}

	static jvalue toC(void* c)
	{
		jvalue v;
		v.c = (jchar) (*(T*) c);
		return v;
	}

	static jvalue toS(void* c)
	{
		jvalue v;
		v.s = (jshort) (*(T*) c);
		return v;
	}

	static jvalue toI(void* c)
	{
		jvalue v;
		v.i = (jint) (*(T*) c);
		return v;
	}

	static jvalue toJ(void* c)
	{
		jvalue v;
		v.j = (jlong) (*(T*) c);
		return v;
	}

	static jvalue toF(void* c)
	{
		jvalue v;
		v.f = (jfloat) (*(T*) c);
		return v;
	}

	static jvalue toD(void* c)
	{
		jvalue v;
		v.d = (jdouble) (*(T*) c);
		return v;
	}

} ;

template <jvalue func(void *c) >
class Reverse
{
public:

	static jvalue call2(void* c)
	{
		char r[2];
		char* c2 = (char*)c;
		r[0]=c2[1];
		r[1]=c2[0];
		return func(r);
	}

	static jvalue call4(void* c)
	{
		char r[4];
		char* c2 = (char*)c;
		r[0]=c2[3];
		r[1]=c2[2];
		r[2]=c2[1];
		r[3]=c2[0];
		return func(r);
	}

	static jvalue call8(void* c)
	{
		char r[8];
		char* c2 = (char*)c;
		r[0]=c2[7];
		r[1]=c2[6];
		r[2]=c2[5];
		r[3]=c2[4];
		r[4]=c2[3];
		r[5]=c2[2];
		r[6]=c2[1];
		r[7]=c2[0];
		return func(r);
	}
} ;


} // namespace

jconverter getConverter(const char* from, int itemsize, const char* to)
{
	// If not specified then the type is bytes
	if (from == NULL)
		from = "B";

	// Skip specifiers
	bool reverse = false;
	unsigned int x = 1;
	bool little = *((char*)&x)==1;
	switch (from[0])
	{
		case '!':
		case '>':
			if (little)
				reverse = true;
			from++;
			break;
		case '<':
			if (!little)
				reverse = true;
			from++;
			break;
		case '@':
		case '=':
			from++;
		default:
			break;
	}

	// Standard size for 'l' is 4 in docs, but numpy uses format 'l' for long long
	if (itemsize == 8 && from[0] == 'l')
		from = "q";
	if (itemsize == 8 && from[0] == 'L')
		from = "Q";

	switch (from[0])
	{
		case '?':
		case 'c':
		case 'b':
			switch (to[0])
			{
				case 'z': return &Convert<int8_t>::toZ;
				case 'b': return &Convert<int8_t>::toB;
				case 'c': return &Convert<int8_t>::toC;
				case 's': return &Convert<int8_t>::toS;
				case 'i': return &Convert<int8_t>::toI;
				case 'j': return &Convert<int8_t>::toJ;
				case 'f': return &Convert<int8_t>::toF;
				case 'd': return &Convert<int8_t>::toD;
			}
			break;
		case 'B':
			switch (to[0])
			{
				case 'z': return &Convert<uint8_t>::toZ;
				case 'b': return &Convert<uint8_t>::toB;
				case 'c': return &Convert<uint8_t>::toC;
				case 's': return &Convert<uint8_t>::toS;
				case 'i': return &Convert<uint8_t>::toI;
				case 'j': return &Convert<uint8_t>::toJ;
				case 'f': return &Convert<uint8_t>::toF;
				case 'd': return &Convert<uint8_t>::toD;
			}
			break;
		case 'h':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<int16_t>::toZ>::call2;
				case 'b': return &Reverse<Convert<int16_t>::toB>::call2;
				case 'c': return &Reverse<Convert<int16_t>::toC>::call2;
				case 's': return &Reverse<Convert<int16_t>::toS>::call2;
				case 'i': return &Reverse<Convert<int16_t>::toI>::call2;
				case 'j': return &Reverse<Convert<int16_t>::toJ>::call2;
				case 'f': return &Reverse<Convert<int16_t>::toF>::call2;
				case 'd': return &Reverse<Convert<int16_t>::toD>::call2;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<int16_t>::toZ;
				case 'b': return &Convert<int16_t>::toB;
				case 'c': return &Convert<int16_t>::toC;
				case 's': return &Convert<int16_t>::toS;
				case 'i': return &Convert<int16_t>::toI;
				case 'j': return &Convert<int16_t>::toJ;
				case 'f': return &Convert<int16_t>::toF;
				case 'd': return &Convert<int16_t>::toD;
			}
			break;
		case 'H':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<uint16_t>::toZ>::call2;
				case 'b': return &Reverse<Convert<uint16_t>::toB>::call2;
				case 'c': return &Reverse<Convert<uint16_t>::toC>::call2;
				case 's': return &Reverse<Convert<uint16_t>::toS>::call2;
				case 'i': return &Reverse<Convert<uint16_t>::toI>::call2;
				case 'j': return &Reverse<Convert<uint16_t>::toJ>::call2;
				case 'f': return &Reverse<Convert<uint16_t>::toF>::call2;
				case 'd': return &Reverse<Convert<uint16_t>::toD>::call2;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<uint16_t>::toZ;
				case 'b': return &Convert<uint16_t>::toB;
				case 'c': return &Convert<uint16_t>::toC;
				case 's': return &Convert<uint16_t>::toS;
				case 'i': return &Convert<uint16_t>::toI;
				case 'j': return &Convert<uint16_t>::toJ;
				case 'f': return &Convert<uint16_t>::toF;
				case 'd': return &Convert<uint16_t>::toD;
			}
			break;
		case 'i':
		case 'l':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<int32_t>::toZ>::call4;
				case 'b': return &Reverse<Convert<int32_t>::toB>::call4;
				case 'c': return &Reverse<Convert<int32_t>::toC>::call4;
				case 's': return &Reverse<Convert<int32_t>::toS>::call4;
				case 'i': return &Reverse<Convert<int32_t>::toI>::call4;
				case 'j': return &Reverse<Convert<int32_t>::toJ>::call4;
				case 'f': return &Reverse<Convert<int32_t>::toF>::call4;
				case 'd': return &Reverse<Convert<int32_t>::toD>::call4;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<int32_t>::toZ;
				case 'b': return &Convert<int32_t>::toB;
				case 'c': return &Convert<int32_t>::toC;
				case 's': return &Convert<int32_t>::toS;
				case 'i': return &Convert<int32_t>::toI;
				case 'j': return &Convert<int32_t>::toJ;
				case 'f': return &Convert<int32_t>::toF;
				case 'd': return &Convert<int32_t>::toD;
			}
			break;
		case 'I':
		case 'L':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<uint32_t>::toZ>::call4;
				case 'b': return &Reverse<Convert<uint32_t>::toB>::call4;
				case 'c': return &Reverse<Convert<uint32_t>::toC>::call4;
				case 's': return &Reverse<Convert<uint32_t>::toS>::call4;
				case 'i': return &Reverse<Convert<uint32_t>::toI>::call4;
				case 'j': return &Reverse<Convert<uint32_t>::toJ>::call4;
				case 'f': return &Reverse<Convert<uint32_t>::toF>::call4;
				case 'd': return &Reverse<Convert<uint32_t>::toD>::call4;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<uint32_t>::toZ;
				case 'b': return &Convert<uint32_t>::toB;
				case 'c': return &Convert<uint32_t>::toC;
				case 's': return &Convert<uint32_t>::toS;
				case 'i': return &Convert<uint32_t>::toI;
				case 'j': return &Convert<uint32_t>::toJ;
				case 'f': return &Convert<uint32_t>::toF;
				case 'd': return &Convert<uint32_t>::toD;
			}
			break;
		case 'q':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<int64_t>::toZ>::call8;
				case 'b': return &Reverse<Convert<int64_t>::toB>::call8;
				case 'c': return &Reverse<Convert<int64_t>::toC>::call8;
				case 's': return &Reverse<Convert<int64_t>::toS>::call8;
				case 'i': return &Reverse<Convert<int64_t>::toI>::call8;
				case 'j': return &Reverse<Convert<int64_t>::toJ>::call8;
				case 'f': return &Reverse<Convert<int64_t>::toF>::call8;
				case 'd': return &Reverse<Convert<int64_t>::toD>::call8;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<int64_t>::toZ;
				case 'b': return &Convert<int64_t>::toB;
				case 'c': return &Convert<int64_t>::toC;
				case 's': return &Convert<int64_t>::toS;
				case 'i': return &Convert<int64_t>::toI;
				case 'j': return &Convert<int64_t>::toJ;
				case 'f': return &Convert<int64_t>::toF;
				case 'd': return &Convert<int64_t>::toD;
			}
			break;
		case 'Q':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<uint64_t>::toZ>::call8;
				case 'b': return &Reverse<Convert<uint64_t>::toB>::call8;
				case 'c': return &Reverse<Convert<uint64_t>::toC>::call8;
				case 's': return &Reverse<Convert<uint64_t>::toS>::call8;
				case 'i': return &Reverse<Convert<uint64_t>::toI>::call8;
				case 'j': return &Reverse<Convert<uint64_t>::toJ>::call8;
				case 'f': return &Reverse<Convert<uint64_t>::toF>::call8;
				case 'd': return &Reverse<Convert<uint64_t>::toD>::call8;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<uint64_t>::toZ;
				case 'b': return &Convert<uint64_t>::toB;
				case 'c': return &Convert<uint64_t>::toC;
				case 's': return &Convert<uint64_t>::toS;
				case 'i': return &Convert<uint64_t>::toI;
				case 'j': return &Convert<uint64_t>::toJ;
				case 'f': return &Convert<uint64_t>::toF;
				case 'd': return &Convert<uint64_t>::toD;
			}
			break;
		case 'f':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<float>::toZ>::call4;
				case 'b': return &Reverse<Convert<float>::toB>::call4;
				case 'c': return &Reverse<Convert<float>::toC>::call4;
				case 's': return &Reverse<Convert<float>::toS>::call4;
				case 'i': return &Reverse<Convert<float>::toI>::call4;
				case 'j': return &Reverse<Convert<float>::toJ>::call4;
				case 'f': return &Reverse<Convert<float>::toF>::call4;
				case 'd': return &Reverse<Convert<float>::toD>::call4;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<float>::toZ;
				case 'b': return &Convert<float>::toB;
				case 'c': return &Convert<float>::toC;
				case 's': return &Convert<float>::toS;
				case 'i': return &Convert<float>::toI;
				case 'j': return &Convert<float>::toJ;
				case 'f': return &Convert<float>::toF;
				case 'd': return &Convert<float>::toD;
			}
			break;
		case 'd':
			if (reverse) switch (to[0])
			{
				case 'z': return &Reverse<Convert<double>::toZ>::call8;
				case 'b': return &Reverse<Convert<double>::toB>::call8;
				case 'c': return &Reverse<Convert<double>::toC>::call8;
				case 's': return &Reverse<Convert<double>::toS>::call8;
				case 'i': return &Reverse<Convert<double>::toI>::call8;
				case 'j': return &Reverse<Convert<double>::toJ>::call8;
				case 'f': return &Reverse<Convert<double>::toF>::call8;
				case 'd': return &Reverse<Convert<double>::toD>::call8;
			}
			else switch (to[0])
			{
				case 'z': return &Convert<double>::toZ;
				case 'b': return &Convert<double>::toB;
				case 'c': return &Convert<double>::toC;
				case 's': return &Convert<double>::toS;
				case 'i': return &Convert<double>::toI;
				case 'j': return &Convert<double>::toJ;
				case 'f': return &Convert<double>::toF;
				case 'd': return &Convert<double>::toD;
			}
			break;
		case 'n':
		case 'N':
		case 'P':
		default: break;
	}
	PyErr_Format(PyExc_TypeError, "Unable to handle buffer type '%s'", from);
	JP_RAISE_PYTHON();
}
