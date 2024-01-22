#include <graphics.h>		// ���� EasyX ��ͼ��ͷ�ļ�
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <map>
#include <vector>
#include <sstream>
#include <cassert>
using namespace std;
//����ȫ�ֵ������������������任�ļ���
struct vec
{
	double x, y, z;
	double eps = 1e-6;
	vec(double a = 0,double b = 0,double c = 0)
	{
		x = a; y = b; z = c;
	}
	//������ȵ��ҽ������������
	bool operator == (const vec& b)
	{
		if (fabs(x - b.x) < eps && fabs(y - b.y) < eps && fabs(z - b.z) < eps)
			return true;
		return false;
	}
	vec operator +(const vec& b)
	{
		return vec(x + b.x,y + b.y,z + b.z);
	}
	vec operator -(const vec& b)
	{
		return vec(x - b.x, y - b.y, z - b.z);
	}
	//��������c
	vec mulc(double c)
	{
		return vec(x * c, y * c, z * c);
	}
};

//vfromobj,����obj�ļ��Ķ���v
/*struct vfromobj
{
	vec v;
	vfromobj(double x = 0, double y = 0, double z = 0)
	{
		v = vec(x,y,z);
	}
};

//vnfromobj,����obj�ļ��Ķ��㷨��vn
struct vnfromobj
{
	vec vn;
	vnfromobj(double x = 0, double y = 0, double z = 0)
	{
		vn = vec(x, y, z);
	}
};
*/
//index pair,���ڸ����洢facet������polygon������index�����ݽṹ;
//facetfromobj,����obj�ļ���facet��������polygon

struct indpair
{
	//v index, vt(������������) index, vn index
	int vi, vti, vni;
	indpair(int a = 0,int b = 0,int c = 0)
	{
		vi = a, vti = b, vni = c;
	}
};
struct facetfromobj
{
	//���ܲ�ֹ3������
	vector<indpair> pointlist;
	//int colorind;
};

struct geometryfromobj
{
	vector<vec> vlist;
	vector<vec> vnlist;
	vector<facetfromobj> flist;

	void read_from_file(string filename = "..\\soccerball.obj")
	{
		//map<int,int>mp;
		fstream fs;
		fs.open(filename, std::fstream::in);
		int linn = 0;
		if (fs.is_open())
		{
			//��ȡobj�ļ�ÿһ��
			while (!fs.eof())
			{
				linn++;
				string a;
				getline(fs, a);
				string b;
				int pos = a.find_first_of(' ');
				if (pos == string::npos)
					continue;
				b.assign(a.substr(0, pos));
				a = a.substr(pos + 1);

				//��ʼ���� aΪָ�bΪ��������
				if (b == "v")
				{
					double x, y, z;
					if(sscanf_s(a.c_str(), "%lf%lf%lf", &x,&y,&z)!=3)
						break;
					vec v(x,y,z);
					vlist.push_back(v);
				}
				else if (b == "vn")
				{
					double x, y, z;
					if(sscanf_s(a.c_str(), "%lf%lf%lf", &x, &y, &z)!=3)
						break;
					vec vn(x, y, z);
					vnlist.push_back(vn);
				}
				else if (b == "f")
				{
					facetfromobj f;
					//�˴�δʹ���������꣬����ֻʹ��x,��z������v��vn
					int x, y, z;
					stringstream ss;
					ss << a;
					//facet��֪���м������㣬�ʲ����ַ�����ѭ������
					while (!ss.eof())
					{
						string tmp="";

						ss >> x;
						tmp.push_back(ss.get());
						tmp.push_back(ss.get());
						ss >> z;
						//����ȡ����
						if (ss.fail()||tmp != "//")
							break;
						//��ȡ�ɹ�
						f.pointlist.push_back(indpair(x, 0, z));
					}
					//facet��Ƭ�洢
					flist.push_back(f);
				}
				
			}
			fs.close();
		}
		print("read_from_file");
	}
	//�����meshתΪtriangle mesh
	void triangle_mesh()
	{
		int n = flist.size();
		for (int i = 0; i < n; i++)
		{
			//����vector��װÿ��facet�²����Ķ�����������Ƭ
			vector<facetfromobj> nfacetlist;
			
			//��ǰfacet�Ķ˵��б�
			vector<indpair>& plst = flist[i].pointlist;
			int pcnt = plst.size();
			if (pcnt > 3)
			{
				//����һ�����������⣬������������Ƭ
				for (int j = 2; j + 1 < pcnt; j++)
				{
					facetfromobj ntriangle;
					ntriangle.pointlist.push_back(plst[0]);
					ntriangle.pointlist.push_back(plst[j]);
					ntriangle.pointlist.push_back(plst[j + 1]);
					nfacetlist.push_back(ntriangle);
				}
				//ֻ������һ�������ε�������
				plst.resize(3);
			}

			//�²����������ηŻ�ԭflist��
			for (int j = 0; j < nfacetlist.size(); j++)
			{
				flist.push_back(nfacetlist[j]);
			}
		}
		print("triangle_mesh");
	}
	//��ӡ�����ǰ����������
	void print(string funcname)
	{
		printf((funcname + "�����\n").c_str());
		printf("��ǰv����%d vn����%d f����%d\n", vlist.size(), vnlist.size(), flist.size());
		map<int, int>mp;
		printf("����Ƭ����=%d\n", flist.size());
		for (int i = 0; i < flist.size(); i++)
		{
			int cnt = flist[i].pointlist.size();
			if (mp.find(cnt) == mp.end())
			{
				mp[cnt] = 0;
			}
			mp[cnt]++;
		}
		for (map<int, int>::iterator a = mp.begin(); a != mp.end(); a++)
		{
			printf("%d����Ƭ��Ϊ%d\n", a->first, a->second);
		}
	}
};

//�������
struct hvec
{
	double xyzw[4];
	double eps = 1e-6;
	hvec(double a = 0, double b = 0, double c = 0,double d = 1)
	{
		xyzw[0] = a; xyzw[1] = b; xyzw[2] = c, xyzw[3] = d;
	}
	//����άvec��������������Ĭ��w=1
	hvec(const vec& b)
	{
		xyzw[0] = b.x; xyzw[1] = b.y; xyzw[2] = b.z;
		xyzw[3] = 1;
	}
	//��������
	hvec(const hvec& b)
	{
		memcpy(xyzw, b.xyzw, sizeof(xyzw));
	}
	//������ȵ��ҽ����ص���ά�����
	bool operator == (const hvec& b)
	{
		for (int i = 0; i < 4; i++)
		{
			if (fabs(xyzw[i] / xyzw[3] - b.xyzw[i] / b.xyzw[3]) >= eps)
				return false;
		}
		return true;
	}
	hvec operator +(const hvec& b)
	{
		return hvec(xyzw[0] + b.xyzw[0], xyzw[1] + b.xyzw[1], xyzw[2] + b.xyzw[2], xyzw[3] + b.xyzw[3]);
	}
	hvec operator -(const hvec& b)
	{
		return hvec(xyzw[0] - b.xyzw[0], xyzw[1] - b.xyzw[1], xyzw[2] - b.xyzw[2], xyzw[3] - b.xyzw[3]);
	}
	//��������c
	hvec mulc(double c)
	{
		return hvec(xyzw[0] * c, xyzw[1] * c, xyzw[2] * c, xyzw[3] * c);
	}
};

//��������Ƭ
struct triangle
{
	//�����˵�����
	indpair ends[3];
	//int color

	//��facetfromobj����
	triangle(const facetfromobj& b)
	{
		if (b.pointlist.size() >= 3)
		{
			for (int i = 0; i < 3; i++)
				ends[i] = b.pointlist[i];
		}
	}
};

//
// 
struct boundingbox
{
	//AABB boundingbox,b[i][0]������iά������Сֵ��b[i][1],��iά�������ֵ
	double b[3][2];
};
//���ڵ���ģ�͵ĺ������ݽṹ��
struct geometry
{
	vector<hvec> vlist;
	vector<hvec> vnlist;
	vector<triangle> flist;

	void assign(const geometryfromobj& b)
	{
		vlist.assign(b.vlist.begin(), b.vlist.end());
		vnlist.assign(b.vnlist.begin(), b.vnlist.end());
		flist.assign(b.flist.begin(), b.flist.end());
	}
	void print(string funcname)
	{
		printf((funcname + "�����\n").c_str());
		printf("��ǰv����%d vn����%d f����%d\n", vlist.size(), vnlist.size(), flist.size());
	}
	/*
	boundingbox apply_transform_and_get_bounding_box(hmat t)
	{
		boundingbox a;
		memset(mm, 0, sizeof(mm));
		for (int i = 0; i < vlist.size(); i++)
		{
			t.
			vlist[i];
			for (int j = 0; j < 3; j++)
			{
				double tt = vlist[i].xyzw[j];
				if (tt > mm[0][j])
					mm[0][j] = tt;
				if (tt < mm[1][j])
					mm[1][j] = tt;
			}
		}
		return
	}*/
};

struct hmat
{
	//4*4�������ϵ�ı任��
	double A[4][4];
	hmat()
	{
		memset(A, 0, sizeof(A));
	}
	hmat(const hmat& b)
	{
		memcpy(A,b.A,sizeof(A));
	}
	//���������
	hvec operator*(const hvec& x)
	{
		hvec b(0, 0, 0, 0);
		for (int i = 0; i < 4; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				b.xyzw[i] += A[i][k] * x.xyzw[k];
			}
		}
		return b;
	}
	//����˾���
	hmat operator*(const hmat& b)
	{
		const double(*B)[4] = b.A;
		hmat c;
		double(*C)[4] = c.A;
		for (int k = 0; k < 4; k++)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					C[i][j] += A[i][k] * B[k][j];
				}
			}
		}
		return c;
	}
};

//�������ͣ��������������ϵ�и������ɵ���ģ��
struct scene {
	//ָ�����ģ�͵�����
	const geometry& base;
	
	vector<hvec> vlist;
	vector<hvec> vnlist;
	vector<triangle> flist;
	//ʹ�÷���任������ģ��
	void make_duplicates(int n)
	{
		//����bounding 
		pow(n, 1.0 / 3);
	}
};



geometryfromobj soccerobj;
geometry soccer;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	FILE* fp;

	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
	/*initgraph(640, 480);	// ������ͼ���ڣ��ֱ��� 640x480
	circle(320, 240, 100);	// ��Բ��Բ�� (320, 240)���뾶 100
	Sleep(5000);			// ��ʱ 5000 ����
	closegraph();			// �ر�ͼ�δ���
	*/
	soccerobj.read_from_file();
	soccerobj.triangle_mesh();
	soccer.assign(soccerobj);
	soccer.print("");

	double mm[2][3];
	memset(mm, 0, sizeof(mm));
	for (int i = 0; i < soccer.vlist.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			double tt = soccer.vlist[i].xyzw[j];
			if (tt > mm[0][j])
				mm[0][j] = tt;
			if (tt < mm[1][j])
				mm[1][j] = tt;
		}
	}
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 3; j++)
			printf(" %f", mm[i][j]);
		printf("\n");
	}
	system("pause");
	return 0;
}
