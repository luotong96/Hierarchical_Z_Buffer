#include <graphics.h>		// ���� EasyX ��ͼ��ͷ�ļ�
#include <iostream>
#include <string>
#include <fstream>
#include <cstdio>
#include <map>
#include <vector>
#include <sstream>
#include <random>
#include <numbers>
#include <list>
#include <chrono>

using namespace std;
//����ȫ�ֵ������������������任�ļ���
struct vec
{
	double xyz[3];
	double eps = 1e-8;
	vec(double a = 0,double b = 0,double c = 0)
	{
		xyz[0] = a; xyz[1] = b; xyz[2] = c;
	}
	vec(double a[])
	{
		xyz[0] = a[0]; xyz[1] = a[1]; xyz[2] = a[2];
	}

	//Ĭ�Ͽ������캯��

	//������ȵ��ҽ������������
	bool operator == (const vec& b)
	{
		for (int i = 0; i < 3; i++)
		{
			if (fabs(xyz[i] - b.xyz[i]) > eps)
				return false;
		}
		return true;
	}

	vec operator +(const vec& b)
	{
		return vec(xyz[0] + b.xyz[0],xyz[1] + b.xyz[1],xyz[2] + b.xyz[2]);
	}

	vec operator -(const vec& b)
	{
		return vec(xyz[0] - b.xyz[0], xyz[1] - b.xyz[1], xyz[2] - b.xyz[2]);
	}

	//��������c
	vec operator *(double c)const
	{
		return vec(xyz[0] * c, xyz[1] * c, xyz[2] * c);
	}

	//�������ȣ�2����
	double norm2()const
	{
		return sqrt(xyz[0]*xyz[0]+xyz[1]*xyz[1]+xyz[2]*xyz[2]);
	}

	//���������ֱ����3������ʽ�Ƴ������ɡ�
	vec cross_product(const vec& b)const
	{
		return vec(xyz[1] * b.xyz[2] - xyz[2] * b.xyz[1], xyz[2] * b.xyz[0] - xyz[0] * b.xyz[2], xyz[0] * b.xyz[1] - xyz[1] * b.xyz[0]);
	}
	//�������
	double dot_product(const vec& b)const
	{
		double ans = 0;
		for (int i = 0; i < 3; i++)
		{
			ans += xyz[i] * b.xyz[i];
		}
		return ans;
	}
	//��������ʽ���ȼ���������������������
	static double determinant(const vec& a,const vec&b, const vec&c)
	{
		return (a.cross_product(b)).dot_product(c);
	}

	static vec get_zero_vector()
	{
		return vec(0, 0, 0);
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
	//Ĭ�Ͽ������캯��
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
	list<facetfromobj> flist;

	void read_from_file(string filename = ".\\soccerball.obj")
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
		//����list��װÿ��facet�²����Ķ�����������Ƭ
		list<facetfromobj> nfacetlist;
		for (list<facetfromobj>::iterator it = flist.begin(); it != flist.end(); it++)
		{			

			//��ǰfacet�Ķ˵��б�
			vector<indpair>& plst = it->pointlist;
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
		}
		flist.splice(flist.end(), nfacetlist);
		print("triangle_mesh");
	}
	//��ӡ�����ǰ����������
	void print(string funcname)
	{
		printf("-------------------------------\n");
		printf((funcname + "�����\n").c_str());
		printf("��ǰ����v����%d ����vn����%d ��Ƭf����%d\n", vlist.size(), vnlist.size(), flist.size());
		map<int, int>mp;
		printf("����Ƭ����=%d\n", flist.size());
		for (list<facetfromobj>::iterator it = flist.begin(); it != flist.end(); it++)
		{
			int cnt = it->pointlist.size();
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
	double eps = 1e-8;
	hvec(double a = 0, double b = 0, double c = 0,double d = 1)
	{
		xyzw[0] = a; xyzw[1] = b; xyzw[2] = c, xyzw[3] = d;
	}
	//����άvec��������������Ĭ��w=1
	hvec(const vec& b)
	{
		for (int i = 0; i < 3; i++)
		{
			xyzw[i] = b.xyz[i];
		}
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
	//��w=1
	void normalize()
	{
		if (fabs(xyzw[3]) < eps)
		{
			//printf("wΪ0���޷���һ��\n");
			for (int i = 0; i < 3; i++)
			{
				xyzw[i] = DBL_MAX;
			}
			return;
		}
		for (int i = 0; i < 3; i++)
		{
			xyzw[i] = xyzw[i] / xyzw[3];
		}
		xyzw[3] = 1;
	}

	//����x,y����������һ������w=1�����ڱ���ͶӰ�任���zֵ��
	void normalize_x_y()
	{
		if (fabs(xyzw[3]) < eps)
		{
			//printf("wΪ0���޷�xy��һ��\n");
			for (int i = 0; i < 2; i++)
			{
				xyzw[i] = DBL_MAX;
			}
			return;
		}
		for (int i = 0; i < 2; i++)
		{
			xyzw[i] = xyzw[i] / xyzw[3];
		}
		xyzw[3] = 1;
	}

	//˦��w���꣬ת��Ϊ3ά����
	vec convert_to_vec()const
	{
		return vec(xyzw[0], xyzw[1], xyzw[2]);
	}
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
		memcpy(A, b.A, sizeof(A));
	}
	//���������
	hvec operator*(const hvec& x)const
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
	hmat operator*(const hmat& b)const
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
	//ƽ������任�ľ���
	static hmat get_translation_hmat(const vec& b)
	{
		hmat T;
		double t[][4] = { {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
		for (int i = 0; i < 3; i++)
			t[i][3] = b.xyz[i];
		memcpy(T.A, t, sizeof(T.A));
		return T;
	}
	//��x����ʱ����תtheta�Ƕ�,��λΪ�����ơ�
	static hmat get_x_axis_rot_hmat(double theta)
	{
		hmat T;
		double t[][4] = { {1,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,1} };
		t[1][1] = t[2][2] = cos(theta);
		t[2][1] = sin(theta);
		t[1][2] = -t[2][1];
		memcpy(T.A, t, sizeof(T.A));
		return T;
	}
	//��y����ʱ����תtheta�Ƕ�,��λΪ�����ơ�
	static hmat get_y_axis_rot_hmat(double theta)
	{
		hmat T;
		double t[][4] = { {0,0,0,0},{0,1,0,0},{0,0,0,0},{0,0,0,1} };
		t[0][0] = t[2][2] = cos(theta);
		t[0][2] = sin(theta);
		t[2][0] = -t[0][2];
		memcpy(T.A, t, sizeof(T.A));
		return T;
	}
	//���쵥λ����
	//static hmat I;
	//static bool constructed;
	static hmat get_unity()
	{
		double I[][4] = { {1,0,0,0},{0,1,0,0 },{0,0,1,0},{0,0,0,1} };
		hmat T;
		memcpy(T.A, I, sizeof(T.A));
		return T;
	}

	void print()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				printf("%f ", A[i][j]);
			}
			printf("\n");
		}
	}
};

//��������Ƭ
struct triangle
{
	//�����˵�����
	indpair ends[3];
	int color;

	//��facetfromobj����
	triangle(const facetfromobj& b)
	{
		if (b.pointlist.size() >= 3)
		{
			for (int i = 0; i < 3; i++)
				ends[i] = b.pointlist[i];
		}
	}
	//Ĭ�Ͽ������캯��
	/*triangle(const triangle& b)
	{
		memcpy(ends,b.ends,sizeof(ends));

	}*/
};

//
// 
struct boundingbox
{
	//AABB boundingbox,b[i][0]������iά������Сֵ��b[i][1],��iά�������ֵ
	double b[3][2];
	boundingbox()
	{
		memset(b, 0, sizeof(b));
	}
	boundingbox(double x, double y, double z, double w, double m, double n)
	{
		b[0][0] = x; b[0][1] = y; b[1][0] = z; b[1][1] = w; b[2][0] = m; b[2][1] = n;
	}
	boundingbox(boundingbox& c)
	{
		memcpy(b, c.b, sizeof(b));
	}
	void print()const
	{
		for (int i = 0; i < 3; i++)
		{
			printf("%f %f|", b[i][0],b[i][1]);
		}
		printf("\n");
	}
	//��̬�ļ������ⶥ�㼯�ϵ�boundingbox,���boundingbox�߽��Ͽ����ж��㡣
	static boundingbox get_bounding_box(const vector<hvec>& vlist)
	{
		boundingbox a;
		memset(a.b, 0, sizeof(a.b));

		for (int i = 0; i < vlist.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				//ȡxyz����ʱҪע�����w
				double tt = vlist[i].xyzw[j] / vlist[i].xyzw[3];
				if (tt < a.b[j][0])
					a.b[j][0] = tt;
				if (tt > a.b[j][1])
					a.b[j][1] = tt;
			}
		}
		return a;
	}

	//����δ��ʵ�����������徫׼�ü������ִ���Ƭ��
	bool is_intersect_with_triangle(const triangle &tri, const vector<hvec>& vlist)const
	{
		return false;
	}

	bool is_contains_triangle(const triangle& tri, const vector<hvec>& vlist)const
	{
		for (int i = 0; i < 3; i++)
		{
			const hvec& p = vlist[tri.ends[i].vi - 1];
			
			//������ÿ����p����ȫ�ڰ�Χ����ŷ����棬�����ڱ߽���

			for (int j = 0; j < 3; j++)
			{
				if (p.xyzw[j] <= b[j][0] || p.xyzw[j] >= b[j][1])
				{
					return false;
				}
			}
		}
		return true;
	}
};



//���ڵ���ģ�͵ĺ������ݽṹ��
struct geometry
{
	vector<hvec> vlist;
	vector<hvec> vnlist;
	list<triangle> flist;

	//��geometryfromobjת��Ϊgeometry
	void assign(const geometryfromobj& b)
	{
		vlist.assign(b.vlist.begin(), b.vlist.end());
		vnlist.assign(b.vnlist.begin(), b.vnlist.end());
		flist.assign(b.flist.begin(), b.flist.end());
		print("assign");
	}
	void print(string funcname)
	{
		printf("-------------------------------\n");
		printf((funcname + "�����\n").c_str());
		printf("��ǰ����v����%d ���㷨��vn����%d ��Ƭf����%d\n", vlist.size(), vnlist.size(), flist.size());
	}
	//��ģ�;���������ԭ��
	void centered()
	{
		//���㵱ǰģ�͵İ�Χ��
		boundingbox bb = boundingbox::get_bounding_box(vlist);
		//�õ���Χ�е��е�
		vec vs(-(bb.b[0][0] + bb.b[0][1])/2, -(bb.b[1][0] + bb.b[1][1]) / 2, -(bb.b[2][0] + bb.b[2][1]) / 2);
		//�õ�����任����
		hmat T = hmat::get_translation_hmat(vs);

		//��ʼ�任
		for (int i = 0; i < vlist.size(); i++)
		{
			vlist[i] = T * vlist[i];
		}
		for (int i = 0; i < vnlist.size(); i++)
		{
			vnlist[i] = T * vnlist[i];
		}
		print("centered");
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



//��������
struct scene {

	//�洢���������ж�����������
	vector<hvec> vlist;
	//�洢���������ж���ķ�������
	vector<hvec> vnlist;
	//�洢����������������Ƭ�Ķ�����
	list<triangle> flist;
	
	void print(string funcname)
	{
		printf("-------------------------------\n");
		printf((funcname + " �����!\n").c_str());
		printf("��ǰ����v����%d ����vn����%d ��Ƭf����%d\n", vlist.size(), vnlist.size(), flist.size());
	}

	//�������ͣ��������������ϵ�и������ɵ���ģ��
	//ģ�ͱ任��ʹ�÷���任�����ƻ���ģ��,base->ָ�����ģ�͵�����,n->����n��
	void generate_from_base(const geometry& base,int n)
	{
		boundingbox bbox = boundingbox::get_bounding_box(base.vlist);


		double range[3];
		//����xyz����������Χrange 
		for (int i = 0; i < 3; i++)
		{
			range[i] = 2 * pow(n, 1.0 / 3)*bbox.b[i][1];
		}

		unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();

		std::default_random_engine generator;
		//std::default_random_engine generator2;

		//��ת�Ƕ�theta�ķֲ�
		std::uniform_real_distribution<double> thetapdf(0, 2 * std::numbers::pi);
		
		//coordinate�����꣩�ķֲ�
		std::uniform_real_distribution<double> coordpdf(-1.0, 1.0);
	
		for (int i = 0; i < n; i++)
		{
			double xtheta = thetapdf(generator);
			double ytheta = thetapdf(generator);
			//��ת����
			hmat R = hmat::get_y_axis_rot_hmat(ytheta) * hmat::get_x_axis_rot_hmat(xtheta);
			
			double coord[3];
			for (int j = 0; j < 3; j++)
			{
				coord[j] = coordpdf(generator)*range[j];
			}

			//����ת����ƽ�ơ�
			hmat T = hmat::get_translation_hmat(vec(coord)) * R;

			//��ÿ��ģ��duplicate��ʼ�任,����v��Ҫ��T�任������vn��Ҫ��R�任������vt(�����)����Ҫ���任��
			//triangle�ж���ı�Ŵ�1��ʼ�����ÿ���µ�duplicate�Ķ�������Ҫ���ϳ��������еĶ���������
			//pvcnt->present v numberĿǰ����scene�е�v������;pvncnt->present vn ������
			int pvcnt = vlist.size();
			int pvncnt = vnlist.size();

			for (int j = 0; j < base.vlist.size(); j++)
			{
				hvec nv = T * base.vlist[j];
				vlist.push_back(nv);
			}

			for (int j = 0; j < base.vnlist.size(); j++)
			{
				hvec nv = R * base.vnlist[j];
				vnlist.push_back(nv);
			}

			for (list<triangle>::const_iterator it = base.flist.cbegin(); it != base.flist.cend(); it++)
			{
				//k < 3,��Ϊÿ��triangle��Ƭ��ֻ���������㡣
				triangle ntriangle = *it;
				for (int k = 0; k < 3; k++)
				{
					//�µĶ�����
					ntriangle.ends[k].vi += pvcnt;
					ntriangle.ends[k].vni += pvncnt;
				}
				ntriangle.color = i + 1;
				//�µ���������Ƭ���ܵ���������
				flist.push_back(ntriangle);
			}
		}
		print("generate_from_base");
	}

};


//2ά��������
struct vec2d
{
	int xy[2];
	vec2d(int a = 0, int b = 0)
	{
		xy[0] = a;
		xy[1] = b;
	}
	bool operator < (const vec2d &b)const
	{
		if (xy[0] != b.xy[0])
			return xy[0] < b.xy[0];
		return xy[1] < b.xy[1];
	}
	vec2d operator - (const vec2d& b)const
	{
		vec2d nv;
		nv.xy[0] = xy[0] - b.xy[0];
		nv.xy[1] = xy[1] - b.xy[1];
		return nv;
	}
	vec2d operator + (const vec2d& b)const
	{
		vec2d nv;
		nv.xy[0] = xy[0] + b.xy[0];
		nv.xy[1] = xy[1] + b.xy[1];
		return nv;
	}
	vec2d operator / (int c)const
	{
		vec2d nv;
		nv.xy[0] = xy[0]/c;
		nv.xy[1] = xy[1]/c;
		return nv;
	}
	bool operator == (const vec2d& b)const
	{
		return (xy[0] == b.xy[0]) && (xy[1] == b.xy[1]);
	}

	//vec���캯���������޷�ʹ�ö����ں����struct�����͡�����ʹ��class������vec�ṹ��ǰ�����������
	//���ƻ�����Ĵ��룬�����ڱ��ṹ����ʹ��convert�����������
	static vec convert_to_vec(const vec2d& b)
	{
		//��ά������Ӧ����ά�������
		return vec(b.xy[0], b.xy[1], 1);
	}
};

//2ά����������
struct box2d
{
	//ends[0]���ԭ���������ends[1]��Զ��ԭ��������������䡣
	vec2d ends[2];

	//�ж�2ά���Ƿ���2d������,�߽�Ҳ��Ϊ����
	bool is_point_within(const vec2d& b)const
	{
		for (int i = 0; i < 2; i++)
		{
			if (b.xy[i] < ends[0].xy[i] || b.xy[i] > ends[1].xy[i])
			{
				return false;
			}
		}
		return true;
	}
	//�жϸ���2ά�����Ƿ��ڱ������ڲ������ӵı߽�����ȡ�����������ꡣ
	bool is_point_within(double xy[2])const
	{
		for (int i = 0; i < 2; i++)
		{
			//�߽粻��Within
			if (xy[i] <= ends[0].xy[i] || xy[i] >= ends[1].xy[i])
				return false;
		}
		return true;
	}
	//�ж�2d�����Ƿ��ڱ������ڲ����߽�Ҳ��Ϊ����
	bool is_box2d_within(const box2d &b)const
	{
		vec2d points[2][2];
		points[1][1] = b.ends[1];
		points[0][0] = points[0][1] = points[1][0] = b.ends[0];
		points[0][1].xy[1] = points[1][1].xy[1];
		points[1][0].xy[0] = points[1][1].xy[0];
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				if (!is_point_within(points[i][j]))
				{
					return false;
				}
			}
		}
		return true;
	}

	//�ж��������Ƿ���ȫ�ڱ������ڲ����߽�Ҳ��Ϊ����
	bool is_triangle_within(const triangle& b, const vector<hvec>& v_in_window_list,int xmax,int ymax)const
	{
		for (int i = 0; i < 3; i++)
		{
			int x = min(xmax - 1, max(0, (int)floor(v_in_window_list[b.ends[i].vi - 1].xyzw[0]+0.5)));
			int y = min(ymax - 1, max(0, (int)floor(v_in_window_list[b.ends[i].vi - 1].xyzw[1]+0.5)));

			if (!is_point_within(vec2d(x,y)))
			{
				return false;
			}
		}
		return true;
	}

	//�жϱ�2d�����Ƿ����������й����㡣ʹ�������������жϡ���double��������ռ��ڣ��й����㵱�ҽ��������ں��Ӷ���������������������ڲ������ߴ��������ζ����ں����ڲ���
	//�������ο�ά�������������½ڡ�
	bool is_intersect_with_triangle(const triangle& b, const vector<hvec>& v_in_window_list, int xmax, int ymax)const
	{

		vec points[3];
		for (int i = 0; i < 3; i++)
		{
			points[i] = vec(v_in_window_list[b.ends[i].vi - 1].xyzw[0], v_in_window_list[b.ends[i].vi - 1].xyzw[1], 1);
			
			
			double xy[2];
			xy[0] = points[i].xyz[0];
			xy[1] = points[i].xyz[1];
			/*����
			//��������νǵ�������2d�����ڣ���һ���ཻ
			int x = min(xmax - 1, max(0, (int)floor(points[i].xyz[0] + 0.5)));
			int y = min(ymax - 1, max(0, (int)floor(points[i].xyz[1] + 0.5)));*/
			
			//�����εĽǵ�Ҫ��ȫ���������ĵ������ڲ������˴����ǻ�����벻���ڵĵ㣬��Ҫ���롣
			if (is_point_within(xy))
			{
				return true;
			}
		}

		//���������ꡣ
		//������abc�����
		double abc = vec::determinant(points[0],points[1],points[2]);
		
		/*����
		//�ĸ������3ά�������,Ϊ��ֹ©��һЩ���أ�ȡ������������0.5��Ϊ�ǵ����ꡣ
		vec corner[2][2];
		corner[0][0] = corner[0][1] = corner[1][0] = (vec2d::convert_to_vec(ends[0])+vec(-0.5,-0.5,0));
		corner[0][1] = corner[0][1] + vec(0, (ends[1] - ends[0]).xy[1] + 1, 0);
		corner[1][0] = corner[1][0] + vec((ends[1] - ends[0]).xy[0] + 1, 0 , 0);
		corner[1][1] = (vec2d::convert_to_vec(ends[1]) + vec(0.5,0.5));
		*/
		
		vec corner[2][2];
		corner[0][0] = corner[0][1] = corner[1][0] = vec2d::convert_to_vec(ends[0]);
		corner[0][1] = corner[0][1] + vec(0, (ends[1] - ends[0]).xy[1] + 1, 0);
		corner[1][0] = corner[1][0] + vec((ends[1] - ends[0]).xy[0] + 1, 0, 0);
		corner[1][1] = vec2d::convert_to_vec(ends[1]);

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				//ϵ��
				double lmd[3];
				//pbc�����/abc��������õ�ϵ����apc,abpͬ��
				lmd[0]  = vec::determinant(corner[i][j], points[1], points[2])/abc;
				lmd[1] = vec::determinant(points[0], corner[i][j], points[2])/abc;
				lmd[2] = vec::determinant(points[0], points[1], corner[i][j])/abc;
				bool in = true;

				for (int k = 0; k < 3; k++)
				{
					//ֻҪ�����������ⲿ����һ���ཻ���������α���Ҳ��Ϊ�ཻ����wiki��������
					if (lmd[k] < 0)
					{
						in = false;
						break;
					}
				}
				if (in)
				{
					return true;
				}
			}
		}
		return false;
	}
};

//���z_buffer
struct node
{
	//����ڵ���֪�����ݡ�
	node* parent;
	//���Ͻ�����coord[0], ���½�����coord[1],
	box2d box;

	//����ڵ�δ֪���������ݡ�kids�ݹ�������⣬z������⡣
	node* kids[2][2];
	double z;
};
struct zpyramid
{
	node* root;
	//ends0Ϊzpyramid�������ԭ��������±�������ends1Ϊ��ԭ��ԭ��������±�������
	zpyramid(const vec2d &ends0,const vec2d &ends1)
	{
		root = new node();
		//���Ͻ�
		root->box.ends[0] = ends0;
		//���½�
		root->box.ends[1] = ends1;
		root->z = -DBL_MAX;
	}
	//�������ص�λ��ӳ�䵽���е�Ҷ�ڵ㡣
	map< vec2d , node* > mp;
	//�ݹ����½���pyramid��㣬���ݹ���zֵС���ѡ�����ֵΪ��Сzֵ��
	double make_up_heap(node* p)
	{
		//printf("%d %d %d %d\n", p->box.ends[0].xy[0], p->box.ends[0].xy[1], p->box.ends[1].xy[0], p->box.ends[1].xy[1]);
		double minz = DBL_MAX;
		//��������ԭ�������ָ�����ϵ�������
		vec2d corner[2][2];
		corner[1][1] = p->box.ends[1];
		corner[0][0] = corner[0][1] = corner[1][0] = p->box.ends[0];
		corner[0][1].xy[1] = corner[1][1].xy[1];
		corner[1][0].xy[0] = corner[1][1].xy[0];

		//�������½�
		if (!(corner[0][0] == corner[1][1]))
		{
			p->kids[0][0] = new node();
			p->kids[0][0]->parent = p;
			p->kids[0][0]->box.ends[0] = corner[0][0];
			p->kids[0][0]->box.ends[1] = (corner[0][0]+corner[1][1])/2;
			minz = fmin(minz, make_up_heap(p->kids[0][0]));
		}
		else
		{
			//��ʱ��ǰ�ڵ�Ϊ�������أ����ɽ�һ����֡�
			minz = -DBL_MAX;
			mp[corner[0][0]] = p;
		}

		//�������½�
		if (!(corner[0][0] == corner[1][0]))
		{
			p->kids[1][0] = new node();
			p->kids[1][0]->parent = p;
			p->kids[1][0]->box.ends[0] = (corner[0][0] + corner[1][0])/2 + vec2d(1,0);
			p->kids[1][0]->box.ends[1] = (corner[1][0] + corner[1][1]) / 2;
			minz = fmin(minz, make_up_heap(p->kids[1][0]));
		}

		//�������Ͻ�

		if (!(corner[0][0] == corner[0][1]))
		{
			p->kids[0][1] = new node();
			p->kids[0][1]->parent = p;
			p->kids[0][1]->box.ends[0] = (corner[0][0] + corner[0][1]) / 2 + vec2d(0,1);
			p->kids[0][1]->box.ends[1] = (corner[0][1] + corner[1][1]) / 2;
			minz = fmin(minz, make_up_heap(p->kids[0][1]));
		}

		//�������Ͻ�
		if (corner[0][0] != corner[0][1] && corner[0][0] != corner[1][0])
		{
			p->kids[1][1] = new node();
			p->kids[1][1]->parent = p;
			p->kids[1][1]->box.ends[0] = (corner[0][0] + corner[1][1]) / 2 + vec2d(1, 1);
			p->kids[1][1]->box.ends[1] = corner[1][1];
			minz = fmin(minz, make_up_heap(p->kids[1][1]));
		}

		return p->z = minz;
	}

	//�õ��������ص�zֵ
	double get_pixel_z(const vec2d& pixel)
	{
		if (mp.find(pixel) == mp.end())
		{
			printf("z_pyramid���Ҳ��������ض�Ӧ�Ľڵ� # %d # %d #\n", pixel.xy[0], pixel.xy[1]);
			return 0;
		}
		return mp[pixel]->z;
	}

	//���µ������ص�z_bufferֵ
	void update(const vec2d& pixel, double nz)
	{
		if (mp.find(pixel) == mp.end())
		{
			printf("update_z_pyramid���Ҳ��������ض�Ӧ�Ľڵ� # %d # %d #\n",pixel.xy[0],pixel.xy[1]);
			return;
		}
		node* p = mp[pixel];
		p->z = nz;
		while (p->parent != NULL)
		{
			node* par = p->parent;
			//������һ����Сֵ���ڵ�z_buffer����Сֵ��
			double nmin = p->z;
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					if (par->kids[i][j] != NULL)
					{
						if (par->kids[i][j]->z < nmin)
						{
							nmin = par->kids[i][j]->z;
						}
					}
				}
			}
			//������³ɹ�����������ϸ��£������˳���
			if (nmin > par->z)
			{
				par->z = nmin;
				p = par;
			}
			else
			{
				break;
			}
		}
	}

	//�жϸ��������Ӿ������Сpyramid�ڵ�,�����ظýڵ�zֵ,aΪ���Ͻǣ�bΪ���½ǡ�
	double min_enclosing_z(const box2d &a, node *p)
	{
		//���Ӿ�����ȫ��p�������ӽڵ㷶Χ�ڣ������µݹ顣���򣬷��ص�ǰp->zֵ��
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				if (p->kids[i][j] == NULL)
					continue;
				if (p->kids[i][j]->box.is_box2d_within(a))
				{
					return min_enclosing_z(a,p->kids[i][j]);
				}
			}
		}
		return p->z;
	}
	//�жϸ���������Ƭ����Сpyramid�ڵ�
	double min_enclosing_z(const triangle& a, const vector<hvec> & v_in_window_list, node* p,int xmax,int ymax)
	{
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				if (p->kids[i][j] == NULL)
					continue;
				if (p->kids[i][j]->box.is_triangle_within(a,v_in_window_list,xmax,ymax))
				{
					return min_enclosing_z(a,v_in_window_list,p->kids[i][j],xmax,ymax);
				}
			}
		}
		return p->z;
	}

	//a��ڵ�p�ཻ�Ĳ����Ƿ�����пɼ��㡣
	//��������ʱ��������һ���뵱ǰ�ڵ��2d�����н�����
	//xmax,ymax->2d��Χ������������+1��triz->������a�����zֵ
	bool is_visible(const triangle& a, const vector<hvec>& v_in_window_list, node* p, double triz ,int xmax, int ymax)
	{
		//����ཻ�Ĳ��ֵ���Сz��������ô�϶����ɼ���
		if (p->z >= triz)
			return false;
		//����Ѿ��ǵ��������ˣ�����ǰһ�������㣬��һ���ɼ���
		if (p->box.ends[0] == p->box.ends[1])
		{
			return true;
		}

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				if (p->kids[i][j] == NULL)
					continue;
				//������������a�ཻ
				if (p->kids[i][j]->box.is_intersect_with_triangle(a,v_in_window_list,xmax,ymax))
				{
					//�ݹ����£�����ཻ�Ĳ��ֻ��пɼ��㣬��ô��ǰ�ڵ�Ҳ�пɼ���
					if (is_visible(a, v_in_window_list, p->kids[i][j], triz,xmax,ymax))
					{
						return true;
					}
				}
			}
		}
		//�ཻ�����в��ֶ����ɼ���
		return false;
	}
};

struct pipeline
{
	//�洢��ǰ���������ж�����������
	vector<hvec> vlist;
	//�洢�任����Ļ����ϵ�󶥵���������
	vector<hvec> v_in_window_list;

	//�洢��ǰ���������ж���ķ�������
	vector<hvec> vnlist;
	//�洢��ǰ����������������Ƭ
	list<triangle> flist;

	//Transform�洢ȡ���任��ͶӰ�任,�Ӵ��任�Ⱥ������б任�ĸ��ϱ任��

	//cuvn�ӵ���������
	hvec c, u, v, n;

	//ͶӰƽ�浽�ӵ�ľ��룬Ӧ��Ϊ������
	double d;

	//ouvn�����У���������̨�Ĵ�С��umax,vmax�ֱ�Ϊ�Ӵ�u,v�������ֵ��front,back��zֵ��Χ�������ĸ����궼Ϊ������������Գơ�
	double umax;
	double vmax;
	double front;
	double back;

	//oxyz�����е������巶Χ����Ϊ������������Գơ�
	double xmax;
	double ymax;
	double zmax;

	//��Ļ�ֱ���
	int xpixelnum;
	int ypixelnum;

	//����8��ֹͣϸ�ֵ�������Ƭ����
	const int stop_triangle_num = 10;


	//��ʾ֡������
	int** framebuffer;

	void print(string funcname)
	{
		printf("-------------------------------\n");
		printf((funcname+"�����\n").c_str());

	}

	//ȡ���任����������任��ouvn����,viewpoint->�ӵ����꣬n->���߷���,up->���ϵķ���
	void view_transform(const vec& viewpoint, vec n, const vec& up, hmat& transform)
	{
		if (n == vec::get_zero_vector() || up == vec::get_zero_vector())
		{
			printf("���߷����up������Ϊ0\n");
			return;
		}
		n = n * (1.0 / n.norm2());
		vec v = n.cross_product(up);
		if (v == vec::get_zero_vector())
		{
			printf("���߷�����up��������һ��ֱ����\n");
			return;
		}
		v = v * (1.0 / v.norm2());
		vec u = v.cross_product(n);
		u = u * (1.0 / u.norm2());

		this->c = viewpoint;
		this->u = u;
		this->v = v;
		this->n = n;

		//��������任����
		hmat T;

		for (int j = 0; j < 3; j++)
			T.A[0][j] = u.xyz[j];
		for (int j = 0; j < 3; j++)
			T.A[1][j] = v.xyz[j];
		for (int j = 0; j < 3; j++)
			T.A[2][j] = n.xyz[j];
		T.A[3][3] = 1;

		hvec c = T * hvec(viewpoint);
		for (int i = 0; i < 3; i++)
			T.A[i][3] = -c.xyzw[i];

		//�����б任����
		transform = T * transform;

		print("viewtransform");
	}

	//��������̨,��Ȼ��ouvn����ϵ��,uΪup����,thetaΪ��Ӧά��������ˮƽ�ߵ����н�,theta<pi/2��
	//front->��������̨ǰ�棬back->��������̨����
	void viewing_frustum(double utheta, double vtheta, double front, double back)
	{
		if (front < 0 || back < 0 || front > back)
		{
			printf("front��back����\n");
			return;
		}
		if (utheta > 0.5 * std::numbers::pi || vtheta > 0.5 * std::numbers::pi || utheta < 0 || vtheta < 0)
		{
			printf("��Ұ�н�utheta��vtheta����\n");
			return;
		}
		double d = (front + back) / 2;
		umax = d * tan(utheta);
		vmax = d * tan(vtheta);
		this->front = front;
		this->back = back;
		this->d = d;
		print("viewing_frustum");
	}

	//͸��ͶӰ�任,dΪͶӰƽ�棬d>0
	void projection(hmat& transform)
	{
		if (d < 0)
		{
			printf("ͶӰƽ��Ӧ�����ӵ�Ϊ������");
			return;
		}

		hmat T = hmat::get_unity();
		T.A[3][2] = 1 / d;
		T.A[3][3] = 0;
		transform = T * transform;

		this->d = d;

		print("projection");
	}


	//����ͨ���������Ӧ�ñ任transform�������У����������䵽vlist��vnlist��flist
	//transfromֱ��Ӧ����v,transformȥ������ƫ�ƺ�Ӧ����vn��ע��͸��ͶӰ��Ӱ�취�����ķ���
	//�ֿ�������֤����ȷ�ԣ�Ax+b/(c'x+d)������ƫ�Ʋ���Ϊb/c'x+d�����ֻ��Ҫ��b=0���ɡ�

	//��һ��Ӧ��transform�任��
	void apply_transform_from_scene(const scene& s, const hmat& transform)
	{
		for (int i = 0; i < s.vlist.size(); i++)
		{
			hvec p = transform * s.vlist[i];
			//��Ҫ����ͶӰ�任֮�󣬰���ʵ��zֵ�����������֤��������ͶӰ�任�ĸ���transformӦ���ڳ�ʼ���ݵ��������󣬵õ���x,y,z������Ȼ���ڽ���������������任�ķ�����
			//��ˣ�����ͶӰ�任֮��transformӦ������ʼ���ݣ��õ���ʵ��zֵ��Ϊ��������̨clipping��׼����
			p.normalize_x_y();
			vlist.push_back(p);
		}

		//bremoved -> ȥ��b�ı任����
		hmat bremoved = transform;
		for (int i = 0; i < 3; i++)
			bremoved.A[i][3] = 0;

		for (int i = 0; i < s.vnlist.size(); i++)
		{
			hvec p = bremoved * s.vnlist[i];
			p.normalize();
			vnlist.push_back(p);
		}

		//��ȿ���triangle��Ƭ
		flist.assign(s.flist.cbegin(), s.flist.cend());

		print("apply_transform_from_scene");
	}

	//�ڱ��������е�������Ӧ��T�任���ǵ�һ��Ӧ�ñ任��
	void apply_transform(const hmat& transform)
	{
		for (int i = 0; i < vlist.size(); i++)
		{
			hvec p = transform * vlist[i];
			vlist[i] = p;
		}

		//bremoved -> ȥ��b�ı任����
		hmat bremoved = transform;
		for (int i = 0; i < 3; i++)
			bremoved.A[i][3] = 0;

		for (int i = 0; i < vnlist.size(); i++)
		{
			hvec p = bremoved * vnlist[i];
			vnlist[i] = p;
		}
	}

	//�ü�����������̨����Ĳ��֡�
	void clipping()
	{

		boundingbox frustum(-umax, umax, -vmax, vmax, front, back);

		for (list<triangle>::iterator it = flist.begin(); it != flist.end(); )
		{

			//�˴����ǵ��ü��㷨�ĸ����ԣ�ֻ������ȫ�������������ڲ���������Ƭ��δ�����Խ���������ά�ü��ڴ˴�Ӧ�á�
			if (!frustum.is_contains_triangle(*it, vlist))
			{
				//ע��erase�ķ���ֵָ��ɾ������һ��Ԫ�ء�
				it = (flist.erase(it));
				continue;
			}
			it++;
		}

		//����ʣ���triangle��ɾ����Ӧ�Ķ�������Ͷ��㷨��ͬʱ�޸�triangle�ж���ı�š�
		vector<hvec> nvlist;
		vector<hvec> nvnlist;
		//map�����ɵ��ŵ��µĵ��ŵ�ӳ���ϵ��
		map<int, int> vmp;
		map<int, int> vnmp;
		for (list<triangle>::iterator it = flist.begin(); it != flist.end(); it++)
		{
			for (int i = 0; i < 3; i++)
			{
				int vi = it->ends[i].vi - 1;
				if (vmp.find(vi) == vmp.end())
				{
					nvlist.push_back(vlist[vi]);
					vmp[vi] = nvlist.size();
				}
				int nvi = vmp[vi];
				it->ends[i].vi = nvi;

				int vni = it->ends[i].vni - 1;
				if (vnmp.find(vni) == vnmp.end())
				{
					nvnlist.push_back(vnlist[vni]);
					vnmp[vni] = nvnlist.size();
				}
				int nvni = vnmp[vni];
				it->ends[i].vni = nvni;
			}
		}

		vlist.assign(nvlist.begin(), nvlist.end());
		vnlist.assign(nvnlist.begin(), nvnlist.end());
		print("clipping");
	}

	//ouvn�任��oxyz��oxyz������ԭ��ŵ�ͶӰƽ���ϣ����ӵ����d��
	void ouvn_to_oxyz(hmat& transform)
	{
		hmat T;
		T.A[0][1] = T.A[1][0] = T.A[3][3] = 1;
		T.A[2][2] = -1;
		T.A[2][3] = d;
		transform = T * transform;
		xmax = vmax;
		ymax = umax;
		//С�ľ�����
		zmax = fmax(d - front, back - d);
		print("ouvn_to_oxyz");
	}

	//�Ӵ��任->�任������Ϊ��λ����Ļ����ϵ,��Ļ����ϵԭ�������Ͻǡ�x����Ϊ����y����Ϊ����
	//��Ļ������Ϊxp*yp,����1024*768
	void window_transform(int xp, int yp, hmat& transform)
	{
		if (xp % 2 && yp % 2)
		{
			printf("��Ļ������ҪΪż��!\n");
			return;
		}

		//һ������
		xp /= 2;
		yp /= 2;
		hmat T;
		//����ռ�����ֱ�����������Ķ��룬ע��˴�������任��
		/*T.A[0][1] = -yp / ymax;
		T.A[1][0] = xp / xmax;
		T.A[0][3] = yp - 0.5;
		T.A[1][3] = xp - 0.5;
		T.A[2][2] = T.A[3][3] = 1;
		*/
		T.A[0][0] = -yp / xmax;
		T.A[1][1] = -xp / ymax;
		T.A[0][3] = yp - 0.5;
		T.A[1][3] = xp - 0.5;
		T.A[2][2] = T.A[3][3] = 1;

		transform = T * transform;
		//print_transform("window_transform");
	}

	//����
	map< vec2d, node* >* surface_visibility(hmat& transform, int xpixelnum, int ypixelnum)
	{
		this->xpixelnum = xpixelnum; this->ypixelnum = ypixelnum;

		//֡����������
		framebuffer = new int* [ypixelnum];
		for (int i = 0; i < ypixelnum; i++)
			framebuffer[i] = new int[xpixelnum]();


		//�������б�Ԥ�ȱ��ݱ任��ͼ������ϵ����scan_convert����ʹ�á�
		hmat T = transform;
		window_transform(xpixelnum, ypixelnum, T);

		for (int i = 0; i < this->vlist.size(); i++)
		{
			hvec tmp = T * this->vlist[i];

			//�任������겻�ܳ���0-n-1�ķ�Χ��
			//tmp.xyzw[0] = min(double(ypixelnum - 1),max(0,floor(tmp.xyzw[0] + 0.5)));
			//tmp.xyzw[1] = min(double(xpixelnum - 1), max(0, floor(tmp.xyzw[1] + 0.5)));

			this->v_in_window_list.push_back(tmp);
		}

		//����zpyramid��ע����Ļ����ϵ��ֱ��ʲ����Ķ�Ӧ��ϵ��
		zpyramid zpy (vec2d(0, 0), vec2d(ypixelnum - 1, xpixelnum - 1));
		zpy.make_up_heap(zpy.root);

		boundingbox bb(-xmax, xmax, -ymax, ymax, -zmax, zmax);

		//�˲��������л᲻�Ͻ�������Ƭlist 8��֣���ʹ��ԭ�е���Ƭ�ı��ݽ������㡣
		list<triangle> mylist(flist.begin(), flist.end());
		octree(transform,bb, mylist, stop_triangle_num, zpy);
		map< vec2d, node* >* msp = new map< vec2d, node* >(zpy.mp);
		print("surface_visibility");
		return msp;
	}


	static bool mycmp(const hvec& a, const hvec& b)
	{
		const double eps = 1e-6;
		if (a.xyzw[1] != b.xyzw[1])
			return a.xyzw[1] > b.xyzw[1];
		return a.xyzw[0] < b.xyzw[0];
	}
	//�������������������������֤��0����maxn-1�ķ�Χ��
	inline static int double_to_legal_int(double a, int maxn)
	{
		return max(0, min(maxn - 1, (int)floor(a + 0.5)));
	}

	//����z_buffer�����˶�����Ƭ
	int cnta = 0;
	int cntb = 0;


	void scan_convert(const list<triangle>& mylist, zpyramid& zpy)
	{
		for (list<triangle>::const_iterator it = mylist.cbegin(); it != mylist.cend(); it++)
		{
			hvec p[3];
			double zmax = -DBL_MAX;
			for (int i = 0; i < 3; i++)
			{
				p[i] = this->v_in_window_list[it->ends[i].vi - 1];
				zmax = fmax(zmax, p[i].xyzw[2]);
			}
			//�ж��Ƿ���Ŀɼ���
			if (!zpy.is_visible(*it, v_in_window_list, zpy.root, zmax, xmax, ymax))
			{
				cntb++;
				continue;
			}

			//ֻ�����˵Ķ���
			for (int i = 0; i < 1; i++)
			{
				int x = double_to_legal_int(p[i].xyzw[0], ypixelnum);
				int y = double_to_legal_int(p[i].xyzw[1], xpixelnum);
				if (p[i].xyzw[2] > zpy.get_pixel_z(vec2d(x,y)))
				{
					//��ǰ���ص�ֵ��Ҫ�����¡�
					framebuffer[x][y] = it->color;
					zpy.update(vec2d(x, y), p[i].xyzw[2]);
				}
			}


			//yֵ�����ǰ�ţ�yֵ��ͬ��xС����ǰ�š� 
			sort(p, p + 3, mycmp);

			//normal ��ǰ��������Ƭ�ķ���
			vec l = (p[1] - p[0]).convert_to_vec();
			vec r = (p[2] - p[0]).convert_to_vec();
			//l������x����ǰ������ֹλ�á�
			double endx = p[1].xyzw[0];

			vec normal = l.cross_product(r);
			if (normal.xyz[2] == 0)
			{
				//ͶӰ��xyƽ���Ϻ����㹲�ߡ�
				continue;
			}
			double dzx = -normal.xyz[0] / normal.xyz[2];
			double dzy = -normal.xyz[1] / normal.xyz[2];

			//�������㹲�ߵ�������⣬ֻ����lˮƽ���ߵڶ���lˮƽ�����廭ͼ���ۡ�
			if (l.xyz[0] == 0 )
			{
				//ˮƽ��
				l = r;
				r = (p[2] - p[1]).convert_to_vec();
				endx = p[2].xyzw[0];
			}

			double dxl = l.xyz[0] / l.xyz[1];
			double dxr = r.xyz[0] / r.xyz[1];

			double xl = p[0].xyzw[0];
			double xr = xl;
			double zl = p[0].xyzw[2];
			double zr = zl;
			
			//topminus1,���������һ���������ꡣ
			int topminus1 =  double_to_legal_int(p[0].xyzw[1],xpixelnum)-1;
			if (topminus1 < 0)
			{
				//�������ڱ�Ե��
				continue;
			}
			double ydelta = topminus1 - p[0].xyzw[1];
			
			//��ǰy����������
			int yp = topminus1;
			
			while (true)
			{
				double xldelta = ydelta * dxl;
				double xrdelta = ydelta * dxr;
				xl += xldelta;
				xr += xrdelta;
				zl += dzx * xldelta + dzy * ydelta;
				zr += dzx * xrdelta + dzy * ydelta;
				
				//Ҳ�����ڴ˴���yp<p[2].xyzw[1]����Ϊ��������
				//�˴�����endx�������жϽ����ͻ�l�����������

				if ((xl - endx) * l.xyz[0] > 0)
				{
					//����������Ҫ��l������
					//����
					if (yp < p[2].xyzw[1])
					{
						break;
					}
					//��l����
					l = (p[2] - p[1]).convert_to_vec();
					dxl = l.xyz[0] / l.xyz[1];
					ydelta = yp - p[1].xyzw[1];
					xldelta = ydelta * dxl;
					
					xl = p[1].xyzw[0] + xldelta;
					zl = p[1].xyzw[2] + dzx * xldelta + dzy * ydelta;
					endx = p[2].xyzw[0];
				}

				pair<double, double> lr[2];
				if (xl < xr)
				{
					lr[0] = make_pair(xl, zl);
					lr[1] = make_pair(xr, zr);
				}
				else
				{
					lr[0] = make_pair(xr, zr);
					lr[1] = make_pair(xl, zl);
				}
				//int roundxl = max(0, min(ypixelnum - 1, (int)ceil(lr[0].first - 1)));
				//int roundxr = max(0, min(ypixelnum - 1, (int)floor(lr[1].first + 1)));
				
				//int roundxl = double_to_legal_int(lr[0].first,ypixelnum);
				//int roundxr = double_to_legal_int(lr[1].first, ypixelnum);

				//left����ȡ����right����ȡ����
				int roundxl = (int)ceil(lr[0].first);
				int roundxr = (int)floor(lr[1].first);

				//��ǰ���ص��zֵ��
				double zz = lr[0].second;
				//ÿ�еĳ�ʼ����x����
				double deltax = roundxl - lr[0].first;
				for (int i = roundxl; i <= roundxr; i++)
				{
					zz += deltax * dzx;
					//��ǰ���ص�x����,y����,zֵ��
					i, yp, zz;
					vec2d points2d(i,yp);
					if (zz > zpy.get_pixel_z(points2d))
					{
						//��ǰ���ص�ֵ��Ҫ�����¡�
						framebuffer[i][yp] = it->color;
						zpy.update(points2d, zz);
					}
					deltax = 1;
				}

				ydelta = -1;
				//�������һ
				yp--;
			}

		}
	}

	//����8���� + ���z_buffer����ʼ���ݹ����£�һ�߽���8������һ��������
	//��oxyz�ռ���8����,��box��������Ƭ��������thresholdʱ��ֹͣ���»��ֿռ䡣transform��Ϊ��ouvn->oxyz
	void octree(hmat &transform, const boundingbox &box,list<triangle> &myflist, int threshold, zpyramid &zpy)
	{
		//��Ҫoctree��ֱ��ɨ��ת����
		//scan_convert(myflist, zpy);
		//return;

		//��֦
		if (myflist.size() == 0)
		{
			return;
		}
		if (myflist.size() < threshold)
		{
			//ɨ��ת��myflist
			scan_convert(myflist, zpy);
			return;
		}

		//��鵱ǰ�ڵ��Ƿ���ȫ�ڵ���
		//��ǰbox���ϽǺ����½���oxyz�ռ���������,��zֵһ�����ǵ�ǰbox�����ӵ���һ���zֵ��
		hvec lefttop(box.b[0][0],box.b[1][1],box.b[2][1]);
		hvec rightdown(box.b[0][1],box.b[1][0],box.b[2][1]);


		hmat T = transform;
		window_transform(xpixelnum, ypixelnum, T);

		lefttop = T * lefttop;
		rightdown = T * rightdown;
		
		//boxinwindow->��ǰ8�����ڵ��Ӧ��cube���ӵ��������cת������Ļ����ϵ���õ���boxͶӰ��2άxyƽ����,ע��˴���С��������ȡ�����ϴ���������ȡ������֤�任���ͶӰ��С����c��ͶӰ��
		box2d boxinwindow;
		
		double minx = min(lefttop.xyzw[0], rightdown.xyzw[0]);
		double maxx = max(lefttop.xyzw[0], rightdown.xyzw[0]);
		double miny = min(lefttop.xyzw[1], rightdown.xyzw[1]);
		double maxy = max(lefttop.xyzw[1], rightdown.xyzw[1]);
		
		int xmin = (int)floor(minx + 0.5);
		if (xmin <= minx)
			xmin += 1;
		int xmax = (int)floor(maxx + 0.5);
		if (xmax >= maxx)
			xmax -= 1;

		int ymin = (int)floor(miny + 0.5);
		if (ymin <= miny)
			ymin += 1;
		int ymax = (int)floor(maxy + 0.5);
		if (ymax >= maxy)
			ymax -= 1;
		/*
		int xmin = double_to_legal_int(floor(minx), ypixelnum);
		int ymin = double_to_legal_int(floor(miny), xpixelnum);
		int xmax = double_to_legal_int(ceil(maxx), ypixelnum);
		int ymax = double_to_legal_int(ceil(maxy), xpixelnum);
		*/
	/*	if (xmin < 0 || ymin < 0 || xmax >= ypixelnum || ymax >= xpixelnum)
		{
			printf("no!!!!!\n");
			system("pause");
		}*/

		boxinwindow.ends[0] = vec2d(xmin, ymin);
		boxinwindow.ends[1] = vec2d(xmax, ymax);

		//����ǰ������ȫ���ɼ������˳���
		double z =  zpy.min_enclosing_z(boxinwindow, zpy.root);

		if (z > lefttop.xyzw[2])
		{
			cnta += myflist.size();
			//printf("###%d\n", cnta);
			//system("pause");
			return;
		}
		

		//����8���Ӻ���
		//3��ά�ȵ���С���м䡢���3��ֵ��
		double points[3][3];
		for (int i = 0; i < 3; i++)
		{
			points[i][0] = box.b[i][0];
			points[i][2] = box.b[i][1];
			points[i][1] = (points[i][0] + points[i][2]) / 2;
		}
		boundingbox subbox[2][2][2];
		//zֵ�Ӵ�Ŀ�ʼ��Ҳ�������ӵ������box
		for (int k = 1; k >= 0; k--)
		{
			//xֵ��С����
			for (int i = 0; i < 2; i++)
			{
				//yֵ��С����
				for (int j = 0; j < 2; j++)
				{
					//boundingbox[i][j]
					subbox[i][j][k] = boundingbox(points[0][i], points[0][i + 1], points[1][j], points[1][j + 1], points[2][k], points[2][k + 1]);
					//subbox[i][j][k].print();
					//system("pause");
				}
			}
		}

		list<triangle> sublist[2][2][2];
		for (list<triangle>::iterator it = myflist.begin(); it != myflist.end();)
		{ 
			bool used = false;
			for (int k = 1; k >= 0; k--)
			{
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						if (subbox[i][j][k].is_contains_triangle(*it,this->vlist))
						{
							sublist[i][j][k].push_back(*it);
							it = myflist.erase(it);
							used = true;
							break;
						}
					}
					if (used)
						break;
				}
				if (used)
					break;
			}
			if (!used)
			{
				it++;
			}
		}

		//ɨ��ת��myflist
		scan_convert(myflist, zpy);

		//���Ŵ����ӽڵ�
		for (int k = 1; k >= 0; k--)
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					octree(transform,subbox[i][j][k], sublist[i][j][k], threshold, zpy);
				}
			}
		}
	}

	void print_framebuffer()
	{
		for (int i = 0; i < ypixelnum; i++)
		{
			for (int j = 0; j < xpixelnum; j++)
			{
				printf("%d ", framebuffer[i][j]);
			}
			printf("\n");
		}
	}
};

vec get_a_coord(string name)
{
	while (1)
	{
		printf( ("������" + name).c_str() );
		double x[3];
		scanf_s("%lf%lf%lf", x, x + 1, x + 2);
		if (x[0] > 100 || x[0] < -100 || x[1] > 100 || x[1] < -100 || x[2] > 100 || x[2] < -100)
		{
			printf("���겻�Ϸ�\n");
			continue;
		}
		else
		{
			return vec(x);
		}
	}
}
void userinterface(vec &o,vec &n, vec&up)
{
	printf("-----------------------------\n");
	printf("�Ƿ���Ҫ����Ĭ���ӵ�����ϵ��1-��Ҫ��0-����Ҫ\n");
	int c;
	while (1)
	{
		scanf_s("%d", &c);
		if (c == 0 || c == 1)
		{
			if (c == 1)
			{
				o = get_a_coord("ԭ�����꣨ʾ����0 0 0��\n");
				n = get_a_coord("�ӵ㷽��ʾ����1 0 0��\n");
				up = get_a_coord("�����up����ʾ����0 0 1��\n");
			}
			break;
		}
		printf("����������\n");
	}
}

geometryfromobj soccerobj;
geometry soccer;

scene soccerfield;
pipeline mypipeline;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//����̨
	FILE* fp;
	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	//���ڼ�ʱ
	using namespace chrono;
	auto start = system_clock::now();

	//��ȡ��׼ģ��
	soccerobj.read_from_file();
	//�������Ƭȫ��ת��Ϊ������Ƭ
	soccerobj.triangle_mesh();
	soccer.assign(soccerobj);

	//ģ�;��е�����ԭ��
	soccer.centered();

	//ģ�ͱ任����1000�����λ�ø��ƻ�׼ģ�ͣ�����������
	soccerfield.generate_from_base(soccer,1000);
	
	auto check1 = system_clock::now();
	printf("ģ�ͱ任��ʱ%f��\n", double(duration_cast<microseconds>(check1 - start).count() * microseconds::period::num / microseconds::period::den));


	//�ӵ�ouvn����ϵ��oo->o,nn->n,up->up��������
	vec oo(0, 0, 0);
	vec nn(1, 0, 0);
	vec up(0, 0, 1);
	
	userinterface(oo, nn, up);



	//ȡ���任
	hmat I = hmat::get_unity();
	mypipeline.view_transform(oo,nn,up,I);
	//��������̨��������
	mypipeline.viewing_frustum(1.0 / 3 * std::numbers::pi, 1.0 / 3 * std::numbers::pi, 1, 1000);
	//ͶӰ�任
	mypipeline.projection(I);
	//��ǰ��ı任����Ӧ�õ������ĵ���,�õ�ouvn�µ����ꡣ
	mypipeline.apply_transform_from_scene(soccerfield,I);
	//������������̨���ü�
	mypipeline.clipping();
	//��ouvn�µĵ�ת����oxyz�����У�������������
	I = hmat::get_unity();
	mypipeline.ouvn_to_oxyz(I);

	printf("Ŀǰ�����д�ɨ��ת����������Ƭ����%d��\n", mypipeline.flist.size());
	
	//��oxyz����ϵ��������,���а��������˲����ĵ��ã����z_buffer�ĵ��ã��Ӵ��任�ĵ��á����Ϊ֡�������е����������
	map< vec2d, node* >* msp = mypipeline.surface_visibility(I,1024,768);
	
	printf("\n�����˲���������������Ƭ���� = %d ���z_buffer������������Ƭ���� = %d\n", mypipeline.cnta, mypipeline.cntb);
	
	auto check2 = system_clock::now();
	printf("ȡ���任��ͶӰ�任���ü�����������ʱ%f��\n", double(duration_cast<microseconds>(check2 - start).count() * microseconds::period::num / microseconds::period::den));


	system("pause");



	// ��ʼ����ͼ����
	initgraph(1024, 768);

	// ��ȡָ����ʾ��������ָ��
	DWORD* pMem = GetImageBuffer();
	
	// ֱ�Ӷ���ʾ��������ֵ
	for (int i = 0; i < 768; i++)
	{
		for (int j = 0; j < 1024; j++)
		{
			if (mypipeline.framebuffer[i][j] != 0)
			{
				pMem[i * 1024 + j] = BGR(RGB(mypipeline.framebuffer[i][j]/16*10, mypipeline.framebuffer[i][j]/4%16*10, mypipeline.framebuffer[i][j] %4*10));
				/*if (mypipeline.framebuffer[i][j] == -1)
				{
					//����Ƕ��㣬�򻻺�ɫ������ʾ��
					pMem[i * 1024 + j] = BGR(RGB(255, 0, 0));
				}
				else
				{
					pMem[i * 1024 + j] = BGR(RGB(255, 255, 255));
				}*/
			}
		}
	}

	/*
	��ÿ�����ص�z_bufferֵ�ó�����ʾ
	for(int i = 0; i < 768; i++)
		for (int j = 0; j < 1024; j++)
		{
			int c = int(msp->at(vec2d(i, j))->z+mypipeline.zmax)%200+50;
			pMem[i * 1024 + j] = BGR(RGB(c,c,c));
		}
	*/
	// ʹ��ʾ��������Ч
	FlushBatchDraw();

	// ������Ƶ�ͼ��
	//saveimage(_T("C:\\Users\\luotong\\Desktop\\test.bmp"));
	saveimage(_T(".\\test.bmp"));
	system("pause");
	closegraph();
	return 0;
}
