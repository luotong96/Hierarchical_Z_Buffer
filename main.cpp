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
using namespace std;
//����ȫ�ֵ������������������任�ļ���
struct vec
{
	double xyz[3];
	double eps = 1e-6;
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
		printf((funcname + "�����\n").c_str());
		printf("��ǰv����%d vn����%d f����%d\n", vlist.size(), vnlist.size(), flist.size());
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
	double eps = 1e-6;
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
			printf("wΪ0���޷���һ��\n");
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
			printf("wΪ0���޷���һ��\n");
			return;
		}
		for (int i = 0; i < 2; i++)
		{
			xyzw[i] = xyzw[i] / xyzw[3];
		}
		xyzw[3] = 1;
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
	//�������캯��
	triangle(const triangle & b )
	{
		memcpy(ends,b.ends,sizeof(ends));
	}
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

	//��̬�ļ������ⶥ�㼯�ϵ�boundingbox
	static boundingbox get_bounding_box(const vector<hvec>& vlist)
	{
		boundingbox a;
		double(*mm)[2] = a.b;
		memset(mm, 0, sizeof(a.b));
		for (int i = 0; i < vlist.size(); i++)
		{
			for (int j = 0; j < 3; j++)
			{
				//ȡxyz����ʱҪע�����w
				double tt = vlist[i].xyzw[j] / vlist[i].xyzw[3];
				if (tt < mm[j][0])
					mm[j][0] = tt;
				if (tt > mm[j][1])
					mm[j][1] = tt;
			}
		}
		return a;
	}

	//����δ��ʵ�����������徫׼�ü������ִ���Ƭ��
	bool is_intersec_with_triangle(const triangle &tri, const vector<hvec>& vlist)const
	{
		return false;
	}

	bool is_contains_triangle(const triangle& tri, const vector<hvec>& vlist)const
	{
		for (int i = 0; i < 3; i++)
		{
			const hvec& p = vlist[tri.ends[i].vi];
			
			//������ÿ����p���ڰ�Χ����ŷ����档

			for (int j = 0; j < 3; j++)
			{
				if (p.xyzw[j] < b[j][0] || p.xyzw[j] > b[j][1])
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
		printf((funcname + "�����\n").c_str());
		printf("��ǰv����%d vn����%d f����%d\n", vlist.size(), vnlist.size(), flist.size());
	}
	//��ģ�;���������ԭ��
	void centered()
	{
		//���㵱ǰģ�͵İ�Χ��
		boundingbox bb = boundingbox::get_bounding_box(vlist);
		//�õ���Χ�е��е�
		vec vs((bb.b[0][0] - bb.b[0][1])/2, (bb.b[1][0] - bb.b[1][1]) / 2, (bb.b[2][0] - bb.b[2][1]) / 2);
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
		printf((funcname + " �����!\n").c_str());
		printf("��ǰv����%d vn����%d f����%d\n", vlist.size(), vnlist.size(), flist.size());
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

		std::default_random_engine generator;
		std::default_random_engine generator2;

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
				coord[j] = coordpdf(generator2)*range[j];
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
};

//���z_buffer
struct node
{
	//����ڵ���֪�����ݡ�
	node* parent;
	//���Ͻ�����coord[0], ���½�����coord[1],
	vec2d coord[2];

	//����ڵ�δ֪���������ݡ�kids�ݹ�������⣬z������⡣
	node* kids[2][2];
	double z;
};
struct zpyramid
{
	node* root;
	zpyramid(int a,int b,int c,int d)
	{
		root = new node();
		//���Ͻ�
		root->coord[0] = vec2d(a,b);
		//���½�
		root->coord[1] = vec2d(c,d);
	}
	//�������ص�λ��ӳ�䵽���е�Ҷ�ڵ㡣
	map< vec2d , node* > mp;
	//�ݹ����½���pyramid��㣬���ݹ���zֵС���ѡ�����ֵΪ��Сzֵ��
	double make_up_heap(node* p)
	{
		double minz = DBL_MAX;

		int x[2][2];
		x[0][0] = (p->coord[0].xy[0] + p->coord[1].xy[0]) / 2;
		x[0][1] = x[0][0] + 1;
		x[1][0] = (p->coord[0].xy[1] + p->coord[1].xy[1]) / 2;
		x[1][1] = x[1][0] + 1;

		//���Ͻ�
		if (!(x[0][1] > p->coord[1].xy[0] && x[1][1] > p->coord[1].xy[1]))
		{
			p->kids[0][0] = new node();
			p->kids[0][0]->parent = p;
			p->kids[0][0]->coord[0] = p->coord[0];
			p->kids[0][0]->coord[1] = vec2d(x[0][0],x[1][0]);
			minz = fmin(minz, make_up_heap(p->kids[0][0]));
		}
		else
		{
			//��ʱ��ǰ�ڵ�Ϊ�������أ����ɽ�һ����֡�
			minz = DBL_MIN;
			mp[p->coord[0]] = p;
		}
		//���Ͻ�
		if (x[1][1] <= p->coord[1].xy[1])
		{
			p->kids[0][1] = new node();
			p->kids[0][1]->parent = p;
			p->kids[0][1]->coord[0] = vec2d(p->coord[0].xy[0],x[1][1]);
			p->kids[0][1]->coord[1] = vec2d(x[0][0], p->coord[1].xy[1]);
			minz = fmin(minz, make_up_heap(p->kids[0][1]));
		}
		//���½�
		if (x[0][1] <= p->coord[1].xy[0])
		{
			p->kids[1][0] = new node();
			p->kids[1][0]->parent = p;
			p->kids[1][0]->coord[0] = vec2d(x[0][1],p->coord[0].xy[1]);
			p->kids[1][0]->coord[1] = vec2d(p->coord[1].xy[0], x[1][0]);
			minz = fmin(minz, make_up_heap(p->kids[1][0]));
		}
		//���½�
		if (x[0][1] <= p->coord[1].xy[0] && x[1][1] <= p->coord[1].xy[1])
		{
			p->kids[1][1] = new node();
			p->kids[1][1]->parent = p;
			p->kids[1][1]->coord[0] = vec2d(x[0][1], x[1][1]);
			p->kids[1][1]->coord[1] = p->coord[1];
			minz = fmin(minz, make_up_heap(p->kids[1][1]));
		}
		return p->z = minz;
	}

	//���µ������ص�z_bufferֵ
	void update(const vec2d& pixel, double nz)
	{
		if (mp.find(pixel) == mp.end())
		{
			printf("z_pyramid���Ҳ��������ض�Ӧ�Ľڵ�\n");
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
			if (nmin < par->z)
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


};

struct pipeline
{
	//�洢��ǰ���������ж�����������
	vector<hvec> vlist;
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

	void print_transform(string funcname)
	{
		printf(("�����"+funcname).c_str());
		//printf(" transform��ǰΪ��\n");
		//transform.print();

	}

	//ȡ���任����������任��ouvn����,viewpoint->�ӵ����꣬n->�ӵ㷽��,up->���ϵķ���
	void view_transform(const vec& viewpoint, vec n, const vec& up,hmat& transform)
	{
		if (n == vec::get_zero_vector() || up == vec::get_zero_vector())
		{
			printf("�ӵ㷽���up������Ϊ0\n");
			return;
		}
		n = n * (1.0 / n.norm2());
		vec v = n.cross_product(up);
		if (v == vec::get_zero_vector())
		{
			printf("���߷�����up��������һ��ֱ����\n");
			return;
		}
		v = v * (1.0/v.norm2());
		vec u = v.cross_product(n);

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

		for (int i = 0; i < 3; i++)
			T.A[i][3] = -viewpoint.xyz[i];

		//�����б任����
		transform = T * transform;

		print_transform("viewtransform");
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

	}

	//͸��ͶӰ�任,dΪͶӰƽ�棬d>0
	void projection(hmat &transform)
	{
		if (d < 0)
		{
			printf("ͶӰƽ��Ӧ�����ӵ�Ϊ������");
			return;
		}

		hmat T = hmat::get_unity();
		T.A[3][2] = 1/d;
		T.A[3][3] = 0;
		transform = T * transform;

		this->d = d;

		print_transform("projection");
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
			p.normalize_x_y();
			vnlist.push_back(p);
		}

		//��ȿ���triangle��Ƭ
		flist.assign(s.flist.cbegin(), s.flist.cend());
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

		boundingbox frustum(-umax,umax,-vmax,vmax,front,back);

		for (list<triangle>::iterator it = flist.begin(); it != flist.end(); )
		{

			//�˴����ǵ��ü��㷨�ĸ����ԣ�ֻ������ȫ�������������ڲ���������Ƭ��δ�����Խ���������ά�ü��ڴ˴�Ӧ�á�
			if (!frustum.is_contains_triangle(*it,vlist))
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
				int vi = it->ends[i].vi;
				if (vmp.find(vi) == vmp.end())
				{
					nvlist.push_back(vlist[vi]);
					vmp[vi] = nvlist.size();
				}
				int nvi = vmp[vi];
				it->ends[i].vi = nvi;

				int vni = it->ends[i].vni;
				if (vnmp.find(vni) == vnmp.end())
				{
					nvnlist.push_back(vnlist[vni]);
					vnmp[vni] = nvnlist.size();
				}
				int nvni = vnmp[vni];
				it->ends[i].vni = nvni;
			}
		}

		vlist.assign(nvlist.begin(),nvlist.end());
		vnlist.assign(nvnlist.begin(), nvnlist.end());
	}   

	//ouvn�任��oxyz��oxyz������ԭ��ŵ�ͶӰƽ���ϣ����ӵ����d��
	void ouvn_to_oxyz(hmat &transform)
	{
		hmat T;
		T.A[0][1] = T.A[1][0] = T.A[3][3] = 1;
		T.A[2][2] = -1;
		T.A[2][3] = d;
		transform = T * transform;
		xmax = vmax;
		ymax = umax;
		//С�ľ�����
		zmax = fmax(d - front,back - d);
		print_transform("ouvn_to_oxyz");
	}

	//�Ӵ��任->�任������Ϊ��λ����Ļ����ϵ,��Ļ����ϵԭ�������Ͻǡ�x����Ϊ����y����Ϊ����
	//��Ļ������Ϊxp*yp,����1024*768
	void window_transform(int xp,int yp,hmat &transform)
	{
		if (xp % 2 && yp % 2)
		{
			printf("��Ļ������ҪΪż��!\n");
			return;
		}
		this->xpixelnum = xp;
		this->ypixelnum = yp;
		//һ������
		xp /= 2;
		yp /= 2;
		hmat T;
		//����ռ�����ֱ�����������Ķ��룬ע��˴�������任��
		T.A[0][1] = -yp / ymax;
		T.A[1][0] = xp / xmax;
		T.A[0][3] = yp - 0.5;
		T.A[1][3] = xp - 0.5;
		T.A[2][2] = T.A[3][3] = 1;

		transform = T * transform;
		//print_transform("window_transform");
	}

	//����8���� + ���z_buffer����ʼ���ݹ����£�һ�߽���8������һ��������
	//��oxyz�ռ���8����
	void octree(const boundingbox &box,int threshold)
	{
		//��鵱ǰ�ڵ��Ƿ���ȫ�ڵ���
		//if()
	}
};

geometryfromobj soccerobj;
geometry soccer;

scene soccerfield;
pipeline mypipeline;
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
	soccer.centered();
	//soccer.print("");

	soccerfield.generate_from_base(soccer,1000);
	
	vec oo(1, 1, 1);
	vec nn(1, 1, 1);
	vec uu(1, 2, 1);
	//mypipeline.viewtransform(oo,nn,uu);
	//mypipeline.projection(5);

	//soccerfield.print("");

	system("pause");
	return 0;
}
