#include <iostream>
#include <string>
#include <bitset>
using namespace std;

typedef bitset<4> word;
typedef bitset<1> byte;
typedef bitset<5> word5;
const int nr = 31;

static word S[16] = { 0x0c,0x05,0x06,0x0b,0x09,0x00,0x0a,     //正确
0x0d,0x03,0x0e,0x0f,0x08,0x04,0x07,0x01,0x02 };

static int P[64] = {
	0,16,32,48,1, //5
	17,33,49,2,18, //5
	34,50,3,19,35, //5
	51,4,20,36, //4
	52,5,21,37,53  //5
	,6,22,38,54,  //4
	7,23,39,55,  //4
	8,24,40,56, //4
	9,25,41,57,10, //5
	26,42,58,11,27,43,59, //7
	12,28,44,60,  //4
	13,29,45,61, //4
	14,30,46,62,//4
	15,31,47,63//4
};

void sBoxLayer(word state[4 * 4])    //S盒置换   
{
	for (int i = 0; i < 16; i++)                     //正确
	{
		int x = state[i][3] * 8 + state[i][2] * 4 + state[i][1] * 2 + state[i][0];
		state[i] = S[x];
	}
};
void PLayer(word in_state[16])  //P盒置换
{
	byte temp_1[64];
	byte temp_2[64];
	word temp;
	for (int z = 0, h = 4; z < 16; z++)
	{
		for (int k = 0; k < 4; k++)
		{
			temp_1[z * h + k] = byte(in_state[z][3 - k]);
		}
		in_state[z] = { 0 };
	}
	for (int i = 0; i < 64; i++)
	{
		temp_2[P[i]] = temp_1[i];
	}
	for (int s = 0, j = 4; s < 16; s++)
	{
		temp = temp_2[j * s].to_ulong();
		temp <<= 3;
		in_state[s] |= temp;
		temp = temp_2[j * s + 1].to_ulong();
		temp <<= 2;
		in_state[s] |= temp;
		temp = temp_2[j * s + 2].to_ulong();
		temp <<= 1;
		in_state[s] |= temp;
		temp = temp_2[j * s + 3].to_ulong();
		in_state[s] |= temp;
	}

};
void generateroundkey(word key[20], word out_key[16], word5 round_counter)
{
	for (int i = 0; i < 16; i++)   //从寄存器K里提取密钥
	{
		out_key[i] = key[i];
	}
	byte temp[80];
	byte temp_3[80];
	for (int i = 0, h = 4; i < 20; i++)  //将4比特字型转换为1比特型
	{
		for (int j = 0; j < 4; j++)
		{
			temp_3[i * h + j] = byte(key[i][3 - j]);
		}
		key[i] = { 0 };
	}
	for (int s = 0; s < 19; s++)   //循环左移61位
		temp[s] = temp_3[s + 61];
	for (int s = 19; s < 80; s++)
		temp[s] = temp_3[s - 19];

	word temp_1;
	word5 temp_2;

	word skey = { 0 };         //k79-k76经过s盒置换
	temp_1 = temp[0].to_ulong();
	temp_1 <<= 3;
	skey |= temp_1;
	temp_1 = temp[1].to_ulong();
	temp_1 <<= 2;
	skey |= temp_1;
	temp_1 = temp[2].to_ulong();
	temp_1 <<= 1;
	skey |= temp_1;
	temp_1 = temp[3].to_ulong();
	skey |= temp_1;
	int x = skey[3] * 8 + skey[2] * 4 + skey[1] * 2 + skey[0];
	skey = S[x];

	word5 skey2 = { 0 };     //k19-k15和论计数器异或
	temp_2 = temp[60].to_ulong();
	temp_2 <<= 4;
	skey2 |= temp_2;
	temp_2 = temp[61].to_ulong();
	temp_2 <<= 3;
	skey2 |= temp_2;
	temp_2 = temp[62].to_ulong();
	temp_2 <<= 2;
	skey2 |= temp_2;
	temp_2 = temp[63].to_ulong();
	temp_2 <<= 1;
	skey2 |= temp_2;
	temp_2 = temp[64].to_ulong();
	skey2 |= temp_2;
	skey2 = skey2 ^ round_counter;

	for (int s = 0; s < 5; s++)
	{
		temp[60 + s] = byte(skey2[4 - s]);
	}

	key[0] = skey;   //完成对密钥寄存器K的更新

	for (int i = 1, h = 4; i < 20; i++)
	{
		temp_1 = temp[i * h].to_ulong();
		temp_1 <<= 3;
		key[i] |= temp_1;
		temp_1 = temp[i * h + 1].to_ulong();
		temp_1 <<= 2;
		key[i] |= temp_1;
		temp_1 = temp[i * h + 2].to_ulong();
		temp_1 <<= 1;
		key[i] |= temp_1;
		temp_1 = temp[i * h + 3].to_ulong();
		key[i] |= temp_1;
	}
};
void keyschedule(word key[16], word state[16])
{
	for (int i = 0; i < 16; i++)
		state[i] = state[i] ^ key[i];
}
int main()
{
	word state1[16] =
	{
		0x0f,0x0f ,0x0f ,0x0f ,
		0x0f,0x0f ,0x0f ,0x0f ,
		0x0f,0x0f ,0x0f ,0x0f ,
		0x0f,0x0f ,0x0f ,0x0f
	};
	word in_key[20] =
	{
		0x0f,0x0f ,0x0f ,0x0f ,
		0x0f,0x0f ,0x0f ,0x0f ,
		0x0f,0x0f ,0x0f ,0x0f ,
		0x0f,0x0f ,0x0f ,0x0f ,
		0x0f,0x0f ,0x0f ,0x0f
	};
	cout << "明文是：\n";
	for (int i = 0; i < 16; i++)
	{
		cout << hex << state1[i].to_ulong() << "  ";
	}
	cout << "\n初始密钥是：\n";
	for (int j = 0; j < 20; j++)
	{
		cout << hex << in_key[j].to_ulong() << "  ";
	}
	word out_key[32][16];
	word key32[16];
	word5 r_counter;
	/*cout << "请输入明文：\n";
	for (int i = 0; i < 16; i++)
	{
		cin >> state1[i];
	}
	cout << "请输入初始密钥：\n";
	for (int i = 0; i <20; i++)
	{
		cin >> in_key[i];
	}*/

	for (int k = 1; k <= nr; k++)
	{	
		cout << k << endl;
		r_counter = word5(k);
		generateroundkey(in_key, out_key[k], r_counter);
	}
	cout << "\n加密进行！\n\n"<<nr<<endl;
	for (int j = 1; j <= nr; j++)
	{
		keyschedule(out_key[j], state1);
		cout << "异或密钥后的密文：" << endl;
		for (int i = 0; i < 16; ++i)
		{
			cout << hex << state1[i].to_ulong() << " ";

		}
		cout << endl;
		cout << "轮密钥：" << endl;
		for (int i = 0; i < 16; ++i)
		{
			cout << hex << out_key[j][i].to_ulong() << " ";

		}

		sBoxLayer(state1);
		cout << endl;
		cout << "S盒后的密文：" << endl;
		for (int i = 0; i < 16; ++i)
		{
			cout << hex << state1[i].to_ulong() << " ";

		}
		PLayer(state1);
		cout << endl;
		cout << "线性层后的密文：" << endl;
		for (int i = 0; i < 16; ++i)
		{
			cout << hex << state1[i].to_ulong() << " ";

		}
		cout << endl;
		cout << endl;
		cout << endl;
		cout << endl;
	}
	/*
	cout << "加密后的密文：" << endl;
	for (int i = 0; i < 16; ++i)
	{
		cout << hex << state1[i].to_ulong() << " ";
		if ((i + 1) % 4 == 0)
			cout << endl;
	}
	*/

	for (int i = 0; i < 16; i++)   //从寄存器K里提取密钥k32
	{
		key32[i] = in_key[i];
	}

		
	keyschedule(key32, state1);
	cout << "异或密钥后的密文：" << endl;
	for (int i = 0; i < 16; ++i)
	{
		cout << hex << state1[i].to_ulong() << " ";

	}


	return 0;

}
