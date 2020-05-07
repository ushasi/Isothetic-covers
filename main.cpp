// hello.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include "cv.h"
#include <iomanip>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
//#include <highgui.h>
#include <conio.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include<vector>
//#include <bits/stdc++.h>


using namespace cv;
using namespace std;
struct point {int x; int y;};
int parent_poly; //vpc-Vertical perimeter component, hpc-horizontal perimeter component

struct neighbour_ugb{
	int type0;
	int cross;
	int boundary;
};
/*struct Point{
	int X,Y;
};*/


struct poly_category{
	int hole;
	int parent_poly;
	int euler;
};
struct point_properties
{
	int no;
	int x;
	int y;
	int m;
	int d;
	point_properties( int no,int x,int y,int m,int d)
  {
	  this->no = no;
      this->x = x;
	  this->y = y;
	  this->m = m;
	  this->d = d;
	}
};

struct Boundingbox{ 
	int topx; 
	int topy;
	int m;
	int boundary_no;
	//vector boundary 
	//position of holes
	//vpc and hpc
    int concavity;
	int EulerNumber; 
	int containment_holes[2];
	string c;

	Boundingbox( int topx,int topy,int m,int boundary_no,int concavity, int EulerNumber,string c)
  {
      
      this->topx = topx;
	  this->topy = topy;
	  this->m = m;
	  this->boundary_no = boundary_no;
	  this->concavity = concavity;
	  this->topx = topx;
      this->EulerNumber = EulerNumber;
	 //*(this->containment_holes) = containment_holes ;
	  this->c = c;
	  
  }

};
vector<Boundingbox> box;
vector<Boundingbox>::iterator iter;
vector<point_properties> boundary_points;
vector<point_properties>::iterator it;
poly_category polygon[100];

int min(int x, int y)
{
  return (x < y) ? x : y;
}

void zero_padding(Mat& img,Mat& img2,int &g)
{
	int borderType;
	Mat dst;
	Scalar value;
	int top, bottom, left, right;
	RNG rng(12345);

	top = (int) (2*g); bottom = (int) (2*g);
    left = (int) (2*g); right = (int) (2*g);
    dst = img;
    borderType = BORDER_CONSTANT;
    value = Scalar( 255,255,255 );
    copyMakeBorder( img, dst, top, bottom, left, right, borderType, value );
	 
	img=dst;
	img2=dst;
	cvtColor( img2, img2, CV_BGR2GRAY );
	cout<<img.rows<<" "<<img.cols<<"\n";
	cout<<img2.rows<<" "<<img2.cols<<"\n";
}

void plot_gridpoints(Mat& img,int g,struct neighbour_ugb **fourugb)
{
	for(int y=0;y<img.rows;y+=g-1)
    {
        for(int x=0;x<img.cols;x+=g-1)
        {
        Vec3b color = img.at<Vec3b>(Point(x,y));
        if(color[0] > 150 && color[1] > 150 && color[2] > 150)
        {
            color[0] = 10;
            color[1] = 10;
            color[2] = 10;

        }
        else
        {
            color.val[0] = 250;
            color.val[1] =250;
            color.val[2] = 250;
			fourugb[y][x].type0=-6;
			//cout<<"("<<y<<" , "<<x<<")";
        }
		img.at<Vec3b>(Point(x,y)) = color;
		}
	}
}
    
int m1_calc(int i,int j,int g,Mat& img2)
{
	int m1=0,flag=0;
	
	for(int x=i-g+1;x<=i;x++)
	  { 
		   for(int y=j-g+1;y<=j;y++)
		   {
			   if(img2.at<uchar>(x,y)==0)
			   {
					m1=1;
					flag=1;
					break;
			   }			
			   if(flag==1)
			   break;
		  }
	  
	  }
	return m1;
}

int m2_calc(int i,int j,int g,Mat& img2)
{
	int  m2=0,flag=0;

	for(int x=i-g+1;x<=i;x++)
	  { 
		   for(int y=j;y<=j+g-1;y++)
		   {
			   if(((x==i-g+1)&&(y==j+g-1))||((x==i)&&(y==j+g-1))||((x==i)&&(y==j))||((x==i-g+1)&&(y==j))){
					continue;
			   }
			   if(img2.at<uchar>(x,y)==0)
			   {
					m2=1;
					flag=1;
					break;
			   }			
			   if(flag==1)
			   break;
		  }
	  
	  }
	return m2;
}

int m3_calc(int i,int j,int g,Mat& img2)
{

	int m3=0,flag=0;
	for(int x=i;x<=i+g-1;x++)
	  { 
		   for(int y=j-g+1;y<=j;y++)
		   {	
			   if(((x==i+g-1)&&(y==j-g+1))||((x==i)&&(y==j-g+1))||((x==i)&&(y==j))||((x==i+g-1)&&(y==j))){
					continue;
			   }
			   if(img2.at<uchar>(x,y)==0)
			   {
					m3=1;
					flag=1;
					break;
			   }			
			   if(flag==1)
			   break;
		  }  
	  }
	return m3;
}

int m4_calc(int i,int j,int g,Mat& img2)
{
	int m4=0,flag=0;
	
	for(int x=i;x<=i+g-1;x++)
	  { 
		   for(int y=j;y<=j+g-1;y++)
		   {
			   if(((x==i)&&(y==j+g-1))||((x==i)&&(y==j))||((x==i+g-1)&&(y==j))){ // ((x==i+g-1)&&(y==j+g-1))||
					continue;
			   }
			   if(img2.at<uchar>(x,y)==0)
			   {
					m4=1;
					flag=1;
					break;
			   }			
			   if(flag==1)
			   break;
		  }
	  
	  }
	return m4;
}

int type_calc(int i,int j,int m1,int m2,int m3,int m4,int& cross,struct neighbour_ugb **fourugb)
{
	int type,m=m1+m2+m3+m4;

		 if(m==0)
		  {type=-2;}
			
	else if(m==1)
		  {type=1;}
			
	else if(m==2)
		  {
			type=0;
			if(  (((m3==1)&&(m2==1)) || ((m1==1)&&(m4==1)))  &&!((m3==1)&&(m2==1)&&(m1==1)&&(m4==1))  )
			  {cross=1;type=1;} 
		  }
	else if(m==3)
	      {type=-1;}
					
	else if(m==4)
		  {type=2;}

	if (cross==1)
	{fourugb[i][j].cross=1;
	cout<<"crossssssssssss"<<i<<" "<<j<<" ";}
	
	return type;
}

void eulerno(int g,Mat& img2,struct neighbour_ugb **fourugb)

{
	cout<<"inside euler no";
	int p=0,k,l,j=0,m1,m3,l_new,flag=0,flag2=0,ctr=0,size;
	size=box.size();
        for (iter = box.begin(); iter != box.end(); ++iter) 
		{
			k=box[p].topx;
			l=box[p].topy;
			//cout<<" "<<k<<" "<<l<"\n";
			//cout<<" "<<ctr;
			for(int i=1;i< 20,(l+i*(g-1))<(img2.rows-2*g);i++)           //////////////////////edit
					{ j=0;flag2=0;
			
						if((fourugb[k][l+i*(g-1)].boundary != 0) && (fourugb[k][l+i*(g-1)].boundary != box[p].boundary_no))  
						{
							ctr=1;
							flag=1;
							l_new=l+i*(g-1);
							m1=m1_calc(k,l_new,g,img2);
							m3=m3_calc(k,l_new,g,img2);
							//cout<<m1<<" "<<m3;
							//cout<<m1<<m3;
							if((m1 == 0) || (m3 == 0) ) // parent polygon spotted || (m3 == 1)
							{
								//cout<<"inside 1111222";
								polygon[p].parent_poly=box[p].boundary_no; cout<<"\nparent polyyyyyyyy "<<k<<" "<<l;break;// itself is a parent polygon
								
							}
							
							else if((m1 != 0) || (m3 != 0) ) // hole polygon spotted
							{
								//cout<<"inside 22222222222";
								//cout<<"\nk= "<<k<<" lnew= "<<l_new<<"\n";
								//cout<<ctr;
								polygon[p].parent_poly=fourugb[k][l_new].boundary;
								//cout<<fourugb[k][l_new].boundary;
								//box[fourugb[k][l].boundary].containment_holes[j]=box[p].boundary_no;   //containment of parent polygons
								j++;
								//cout<<box[fourugb[k][l].boundary].containment_holes[j];
								flag=1;
								if (p == size-1)
								{flag2=1;}
								break;
							}
						
						}
						ctr++;
						//cout<<" ctr "<<ctr;
						if (ctr >=10 && flag == 0)
							{cout<<"inside 33333333";polygon[p].parent_poly=box[p].boundary_no; cout<<"parent polyyyyyyyy "<<k<<" "<<l<<"\n";ctr=0;break;}// itself is a parent polygon
			}
			p++;
		}
		return;
}

void concavity(int g,Mat& img2,struct neighbour_ugb **fourugb,int counter3)
{ //vector<point_properties> boundary_points;
	int pre,cur,p=0,d,counter=1;

	int* bottom_concavity = new int[counter3];
	int* right_concavity = new int[counter3]; 
	int* up_concavity = new int[counter3]; 
	int* left_concavity = new int[counter3]; 

for (int i=0; i<counter3; i++) {
    bottom_concavity[i] = 0; 
	right_concavity[i] = 0; 
	up_concavity[i] = 0; 
	left_concavity[i] = 0; 
}
int a=0,b=0,c=0,x=0;
cout<<"inside concavity";
	 for (it = boundary_points.begin(); it != boundary_points.end(); ++it) 
		{a++;b++;c++;x++;
	 if (polygon[p].hole == 1) break;
			while (boundary_points[p].no==counter)
			{
				//cout<<p<<" ";
				if( boundary_points[p+1].no != boundary_points[p].no)
				p++;

			pre= boundary_points[p].m;
		
			while (pre==3 && boundary_points[p+1].m == 2)
			{p=p+1;
			if(p == boundary_points.size()-2   )
				{cout<<"breaking concavity\n";
			break;}
			}
			cur = boundary_points[p+1].m;

			d=boundary_points[p].d;
			//cout<<counter<<"."<<cur<<" ";

			if (pre ==3 && cur == 3 && d==0)
			{cout<<" bottom "<<boundary_points[p].no;bottom_concavity[a-1]+=1;}

			if (pre ==3 && cur == 3 && d==1)
			{cout<<" right "<<boundary_points[p].no;right_concavity[b-1]+=1;}

			if (pre ==3 && cur == 3 && d==2)
			{cout<<" up "<<boundary_points[p].no;up_concavity[c-1]+=1;}

			if ((pre ==3) && (cur == 3) && (d==3))
			{cout<<" left "<<boundary_points[p].no;left_concavity[x-1]+=1;}

			//cout<<p;
			p++;
			if(p == boundary_points.size()-2 )
				{cout<<"breaking concavity\n";
			break;}

			}
			counter++;
			pre=0;
	 }
	 return;
}

void convexity(int g,Mat& img2,struct neighbour_ugb **fourugb,int counter3)
{ //vector<point_properties> boundary_points;
	int pre,cur,p=0,d,counter=1;
   
	cout<<"inside convexity";
	int* bottom_convexity = new int[counter3];
	int* right_convexity = new int[counter3]; 
	int* up_convexity = new int[counter3]; 
	int* left_convexity = new int[counter3]; 

for (int i=0; i<counter3; i++) {
    bottom_convexity[i] = 0; 
	right_convexity[i] = 0; 
	up_convexity[i] = 0; 
	left_convexity[i] = 0; 
}
int a=0,b=0,c=0,x=0;
	 for (it = boundary_points.begin(); it != boundary_points.end(); ++it) 
		{a++;b++;c++;x++;
			while (boundary_points[p].no==counter)
			{
				if( boundary_points[p+1].no != boundary_points[p].no)
				p++;

			pre= boundary_points[p].m;
		
			while (pre==1 && boundary_points[p+1].m == 2)
			{p=p+1;
			if(p == boundary_points.size()-2   )
				{cout<<"breaking concavity\n";
			break;}
			}
			cur = boundary_points[p+1].m; 
			d=boundary_points[p].d;
			//cout<<counter<<"."<<cur<<" ";

			if (pre ==1 && cur == 1 && d==0)
			{cout<<"bottom"<<boundary_points[p].no;bottom_convexity[a-1]+=1;}

			if (pre ==1 && cur == 1 && d==1)
			{cout<<"right"<<boundary_points[p].no;right_convexity[b-1]+=1;}

			if (pre ==1 && cur == 1 && d==2)
			{cout<<"up"<<boundary_points[p].no;up_convexity[c-1]+=1;}

			if ((pre ==1) && (cur == 1) && (d==3))
			{cout<<"left"<<boundary_points[p].no;left_convexity[x-1]+=1;}

			if(p == boundary_points.size()-2)
				{cout<<"breaking convexity\n";
			break;}

			p++;
			}
			counter++;
			pre=0;
	 }
	 return;
}

/*void 3convex(int g,Mat& img2,struct neighbour_ugb **fourugb,int counter3)
{ 
	int pre,cur,p=0,d,counter=1;
	cout<<"inside convexity";
	int* bottom_convexity = new int[counter3];
	int* right_convexity = new int[counter3]; 
	int* up_convexity = new int[counter3]; 
	int* left_convexity = new int[counter3]; 

for (int i=0; i<counter3; i++) {
    bottom_convexity[i] = 0; 
	right_convexity[i] = 0; 
	up_convexity[i] = 0; 
	left_convexity[i] = 0; 
}
int a=0,b=0,c=0,x=0;
	 for (it = boundary_points.begin(); it != boundary_points.end(); ++it) 
		{a++;b++;c++;x++;
			while (boundary_points[p].no==counter)
			{
				if( boundary_points[p+1].no != boundary_points[p].no)
				p++;

			pre= boundary_points[p].m;
		
			while (pre==1 && boundary_points[p+1].m == 2)
			{p=p+1;
			if(p == boundary_points.size()-2   )
				{cout<<"breaking concavity\n";
			break;}
			}
			cur = boundary_points[p+1].m; 
			d=boundary_points[p].d;
			//cout<<counter<<"."<<cur<<" ";

			if (pre ==1 && cur == 1 && d==0)
			{cout<<"bottom"<<boundary_points[p].no;bottom_convexity[a-1]+=1;}

			if (pre ==1 && cur == 1 && d==1)
			{cout<<"right"<<boundary_points[p].no;right_convexity[b-1]+=1;}

			if (pre ==1 && cur == 1 && d==2)
			{cout<<"up"<<boundary_points[p].no;up_convexity[c-1]+=1;}

			if ((pre ==1) && (cur == 1) && (d==3))
			{cout<<"left"<<boundary_points[p].no;left_convexity[x-1]+=1;}

			if(p == boundary_points.size()-2)
				{cout<<"breaking convexity\n";
			break;}

			p++;
			}
			counter++;
			pre=0;
	 }
	 return;
}*/

void  hole_position(Mat& img,int g,int counter3)
{
	int min_x=100000, min_y=100000,max_x=FLT_MIN, max_y=FLT_MIN,p=0,j=1,flag2=0,center_x[100],center_y[100];
	float poh_ratio[100]={0.0},height[100]={0.0};

	cout<<"inside hole position";
	 for (it = boundary_points.begin(); it != boundary_points.end(); ++it) 
		{min_x=100000; min_y=100000;max_x=FLT_MIN;max_y=FLT_MIN;
		if (j > box.size())
				{flag2=1;
				break;}
			while (boundary_points[p].no == j)
			{
				min_x = min(min_x,boundary_points[p].x);
				min_y = min(min_y,boundary_points[p].y);
				max_x = max(max_x,boundary_points[p].x);
				max_y = max(max_y,boundary_points[p].y);
				p++;
				if (p >boundary_points.size()-1)
				{flag2=1;
				break;}
			}
				//cout<<j<<" "<<min_x<<" "<<min_y<<"\n";
				//cout<<j<<" "<<max_x<<" "<<max_y<<"\n";
				height[j-1]=max_x - min_x;
			    center_x[j-1] = (min_x+max_x)/2;
				center_y[j-1] = (min_y+max_y)/2;
				//cout<<center_x[j-1]<<" "<<center_y[j-1]<<"\n";
				j++;
				cout<<"\n";
				//circle(img, Point(center_x,center_y), 5, Scalar( 255, 0, 0 ), 1, 8, 0);
	    }
	 	/* if (box[p].boundary_no  != polygon[p].parent_poly  )
	 {cout<<"turuuu"<<box[p].boundary_no ;
	 polygon[p].hole=1;*/
	  p=0;
	 for(int i=0;i<counter3;i++)
	 {if(  polygon[p].hole==1)
	 {
		 cout<<polygon[p].parent_poly ;
		 cout<<height[polygon[p].parent_poly-1];
		 cout<<"boundary "<<center_y[polygon[p].parent_poly-1] ;
		 cout<<" hole "<<center_y[p] ;
		 double yo;
		 yo=(center_y[polygon[p].parent_poly-1] - center_y[p] )*10 ;
		 poh_ratio[p]=yo / (height[polygon[p].parent_poly-1]);
		 //cout<<"yuyhuoufobo"<<center_y[polygon[p].parent_poly-1] - center_y[p];
		// cout<<height[polygon[p].parent_poly-1];
		 cout<<endl<<poh_ratio[p]<<endl;
	 }
	 p++;
	 }

	return;
}

 void VertHoriPerimeterComp(int counter3)
 {
	 int j=1,p=0,vpc[100]={0},hpc[100]={0},flag2=0;
	 cout<<"inside vpc and hpc";

	 for (it = boundary_points.begin(); it != boundary_points.end(); ++it) 
		{
			while (boundary_points[p].no == j)
			{
				if ((boundary_points[p].d == 1) || (boundary_points[p].d == 3))
					vpc[j-1]=vpc[j-1]+1;
				else
					hpc[j-1]=hpc[j-1]+1;
				p++;
				
				if (p > boundary_points.size()-1)
				{flag2=1;
				break;}
				
			}
				cout<<j<<" "<<vpc[j-1]<<" "<<hpc[j-1]<<"\n";
				//float yo=vpc[0]+hpc[0];
				if (flag2 == 1)
					break;
				j++;		
	    }
	 return;
 }

void VertDirectionChange(int counter3)
{
	int flag2=0,vdc[100]={0},j=1,p=0,x=0;
	cout<<"inside vdc";

	for (it = boundary_points.begin(); it != boundary_points.end(); ++it) 
		{x=0;
			while (boundary_points[p].no == j)
			{
				if ((((x==0)||(x==3)) && boundary_points[p].d == 1) || (((x ==0)||(x==1))&& boundary_points[p].d == 3))
				{vdc[j-1]=vdc[j-1]+1;
				x=boundary_points[p].d;
				}
				p++;
				
				if (p > boundary_points.size()-1)
				{flag2=1;
				break;}
				
			}
				cout<<j<<" "<<vdc[j-1]<<"\n";
				if (flag2 == 1)
					break;
				j++;		
	    }
	 return;
 }



/**********************************************************************************************************************************************/


int _tmain(int argc, _TCHAR* argv[])
{
	int a,b,g,index=0,hole_count=0,polygons[50]={0};
	Mat img;
	Mat img2;

    char pathname[]="C://Users//Public//Pictures//atoz.png";

	 // C://Users//Public//Pictures//Sample Pictures//

    ofstream myfile;
    myfile.open ("example.txt");
    myfile << "Writing this to a file.\n";
 
	img= imread(pathname,1); //image path
	cout<<"enter the value of g";
	cout<<img.rows<<" "<<img.cols;
	int gnew=min(img.rows,img.cols);
	g=floor((double)(gnew/20));
	abc:
	g=floor((double)g-2);
	//g=8;
	if(g<5)
	goto xyz;
	cout<<" suggested g          (" <<g<<")             ";
	img= imread(pathname,1); //image path
	img2= imread(pathname,0); //image path
	//cvtColor( img, img2, CV_BGR2GRAY );

    box.clear();
	boundary_points.clear();
	memset(polygon, 0, sizeof(poly_category));
	cout<<sizeof(poly_category);
	//polygon[100]={0}
	//delete[] polygon;
	int counter=0;
	zero_padding(img,img2,g);

	  //*************************************************************************************************

	int rows=img.rows;
    int cols=img.cols;

	neighbour_ugb **fourugb=new (nothrow) neighbour_ugb*[rows];
			 for(int row = 0; row < rows; row++)
				fourugb[row] = new neighbour_ugb[cols];

	for(int y=0;y<img2.rows;y++)
    {
        for(int x=0;x<img2.cols;x++)
        {
			fourugb[y][x].type0=-10;
			
		}
	}

	for(int y=0;y<img2.rows;y+=g-1)
    {
        for(int x=0;x<img2.cols;x+=g-1)
        {
			if(fourugb[y][x].type0 !=-6)
			{fourugb[y][x].type0=-5;
			fourugb[y][x].cross=0;
			fourugb[y][x].boundary=0;}
			//cout<<fourugb[y][x].type0;
		}
	}

	plot_gridpoints(img,g,&*fourugb);


//**************To find the first point****************************************
int counter3=0;
for(int count=1;count<100;count++)

{
	cout<<endl<<endl<<endl<<endl;
	int k,l,k_count,l_count,flag=0,flag2=0,flag3=0,flag4=0,x1=0,y1=0,hole,counter2=0,m1=0,m2=0,m3=0,m4=0,m=0,is,js;
	k=0;l=0;
	k_count=g;
	l_count=g;

	int xs=-1;
	int ys=-1;
	flag=0;
	 
	 //counter2++;
	//vector <poly_category> polygon;
   // vector <poly_category> poly;
	int tile_row=img2.rows /(g-1);
	int tile_col=img2.cols /(g-1);

	for(int k=0;k<img2.rows-g;k+=g-1)
	{
		for(int l=0;l<img2.cols-g;l+=g-1)
		{
			for(int y=0;y<g;y++)
			{ 
				 for(int x=0;x<g;x++)
				{
					if((img2.at<uchar>(k+x,l+y)==0)&& (fourugb[k][l].boundary == 0) &&(fourugb[k][l-g+1].boundary == 0)&& (fourugb[k-g+1][l].boundary == 0) && (fourugb[k+g-1][l].boundary == 0)&& (fourugb[k][l+g-1].boundary == 0) && ((fourugb[k][l].type0 == -5) ||(fourugb[k][l].cross == 1))&&  (fourugb[k-g+1][l].type0 != -6)&&(fourugb[k-g+1][l+g-1].type0 != -6) )//&&(fourugb[k-g+1][l-g+1].type0 != -6)//&& (fourugb[k][l].type0 != -6)
					{   cout<<"hi3 "<<" type  "<<fourugb[k][l].type0<<"   cross "<<fourugb[k][l].cross;
						fourugb[k][l].type0--;
						xs=x+k;
						ys=y+l;
						cout<<" k="<<k<<" l="<<l;
						//polygon[counter2]=eulerno(k,l,g,&*fourugb);
						//polygon.push_back(poly);
						is = (ceil((double)xs/(g-1))-1)*(g-1);
						js = (floor((double)ys/(g-1))+1)*(g-1);
						js -= g-1;
						m1=m1_calc(is,js,g,img2);
						m2=m2_calc(is,js,g,img2);
						m3=m3_calc(is,js,g,img2);
						m4=m4_calc(is,js,g,img2);
						m=m1+m2+m3+m4;
						cout<<"mmmmmm.........."<<m<<" ";

						if ((fourugb[k][l].boundary != 0) && (fourugb[k][l].cross != 1))
							continue;
						if ((m == 2) || (m ==4))
							continue;

						fourugb[k][l].boundary=counter+1;
						cout<<"BOUNDARYYYYYYYYYY" <<fourugb[k][l].boundary;
						flag=1;
						int arr[5]={0};
						box.push_back(Boundingbox( is,js,m,counter+1,0,1,"null")); //Boundingbox( int topx,int topy,int boundary_no,int concavity, int EulerNumbar,string c,)

						box[counter].containment_holes[0]=0;
						box[counter].containment_holes[1]=0;
						polygon[counter].hole=0;
						polygon[counter].parent_poly=counter+1;
						polygon[counter].euler=1;
						//cout<<"polygon[counter].parent_poly"<<polygon[counter].parent_poly;

					}
				   if(flag==1)
				   { flag2=1;
				   break;}
				}
				 if(flag2==1)
				   { flag3=1;
				   break;}

			}	
			if(flag3==1)
			   { flag4=1;
			   break;}
		}
	}
	if(xs == -1 && ys == -1){
		break;
	}
	counter3++;
	cout<<"\n POLYGON NUMBER: "<<counter3;
	//cout<<"\n POLYGON NUMBER: "<<counter3<<". "<<polygon[counter2].euler<<" "<<polygon[counter2].hole<<" "<<polygon[counter2].parent_poly<<"\n";
	cout<<"\n"<<" x start= "<<xs<<" y start= "<<ys<<"\n";	
	//int is = (ceil((double)xs/(g-1))-1)*(g-1);
	//int js = (floor((double)ys/(g-1))+1)*(g-1);
	//js -= g-1;
	//if ((fourugb[is][js].boundary != 0) && (fourugb[is][js].cross != 1)

	cout<<"is  "<<is<<" "<<"js  "<<js<<"\n";
	if(polygon[counter-1].hole != 1)
		{counter++;
		//character_poly[counter-1][0] = is;
		//character_poly[counter-1][1] = js;
		//cout<<"charac "<<character_poly[counter-1][0]<<" "<<character_poly[counter-1][1];
		}

	else {hole_count++;
		//hole_poly[hole_count-1][0]= is;
		//hole_poly[hole_count-1][1]= js;
		//character_poly[counter-1][2] =parent_poly;
		//cout<<"hole "<<hole_poly[hole_count-1][0]<<" "<<hole_poly[hole_count-1][1];
	}

	//***********************************to find type**************************************
	int i=is;
	int j=js;
	flag=0;
		
	//*************************************************************************************
	int type,cross=0;

    m1=m1_calc(i,j,g,img2);
	cout<<" m1="<<m1;

	m2=m2_calc(i,j,g,img2);
	cout<<" m2="<<m2;

	m3=m3_calc(i,j,g,img2);
	cout<<" m3="<<m3;
	
	m4=m4_calc(i,j,g,img2);
	cout<<" m4="<<m4;

	m=m1+m2+m3+m4;
	cout<<",  m="<<m<<" ";	

    type=type_calc(i,j,m1,m2,m3,m4,cross,&*fourugb);
	fourugb[i][j].type0=type;
	fourugb[i][j].boundary=counter;

	cout<<"type "<<type<<endl<<"    boundary "<<fourugb[i][j].boundary;

	int ts=type;
	int ds=(2+ts)%4;
	int d=ds;
		boundary_points.push_back(point_properties( counter,i,j,m,d));
	int x_cur=i;
	int y_cur=j;

	if (d==0){
		j=j+g-1;
	}
	if (d==1){
		i=i-g+1;
	}
	if (d==2){
		j=j-g+1;
	}
	if (d==3){
		i=i+g-1;
	}
	if ( fourugb[i][j].boundary != 0)
		continue;
	cout<<" d= "<<d;
	cout<<" i= "<<i;
	cout<<" j= "<<j;
	int x_new,y_new;
	x_new=i;
	y_new=j;


	if ( fourugb[i][j].boundary != 0  )
line( img, Point( y_new,x_new ), Point( y_cur,x_cur), Scalar( 200, 0, 100 ),  1, 4 );
		
else 
	line( img, Point( y_new,x_new ), Point( y_cur,x_cur), Scalar( 255, 0, 0 ),  1, 4 );
	myfile<<g<<" "<<y_new<<" "<<x_new<<" "<<ts<<"\n";

	x_cur=x_new;
	y_cur=y_new;

	//***********************************************************************************


	for(int f=0;f>-1;f++)
	{
		int m1=0,m2=0,m3=0,m4=0,m=0,type,cross,flag=0;

		for(int x=i-g+1;x<=i;x++)
		  { 
			   for(int y=j-g+1;y<=j;y++)
			   {
				   cross=0;
				    if(((x==i-g+1)&&(y==j-g+1))||((x==i)&&(y==j-g+1))||((x==i)&&(y==j))||((x==i-g+1)&&(y==j))){
					continue;
			   }
				   if(img2.at<uchar>(x,y)==0)
				   {
						m1=1;
						flag=1;
						break;
				   }			
				   if(flag==1)
				   break;
			  }
		  
		  }

		flag=0;
		
	m2=m2_calc(i,j,g,img2);
	m3=m3_calc(i,j,g,img2);
	m4=m4_calc(i,j,g,img2);
	m=m1+m2+m3+m4;
		  
		  type=type_calc(i,j,m1,m2,m3,m4,cross,&*fourugb);
		  fourugb[i][j].type0=type;
		  fourugb[i][j].boundary=counter;
		  //cout<<"type "<<fourugb[i][j].type0<<endl<<"    boundary  "<<fourugb[i][j].boundary;
		  //imshow("yolo",img2);

		int dc=d;
		int tn=type;
		int d_intermediate=(dc+tn);

		if(d_intermediate<0)
		d=4+d_intermediate;
		else d=d_intermediate%4;

			boundary_points.push_back(point_properties( counter,i,j,m,d));
		if (d==0){
			j=j+g-1;
		}
		if (d==1){
			i=i-g+1;
		}
		if (d==2){
			j=j-g+1;
		}
		if (d==3){
			i=i+g-1;
		}
		//cout<<" d= "<<d;
		//cout<<" i= "<<i;
		//cout<<" j= "<<j;
		//if ( fourugb[i][j].boundary != 0)
		//continue;
		x_new=i;
		y_new=j;
		//if ((fourugb[y_new-g+1][x_new].type0==-6)&&(fourugb[y_cur-g+1][x_cur].type0==-6) )
			//break;


if ( fourugb[i][j].boundary != 0  )
line( img, Point( y_new,x_new ), Point( y_cur,x_cur), Scalar( 200, 0, 100 ),  1, 4 );
		
else 
line( img, Point( y_new,x_new ), Point( y_cur,x_cur), Scalar( 10, 255, 0 ),  1, 4 );
		myfile<<g<<" "<<y_new<<" "<<x_new<<" "<<tn<<"\n";
		x_cur=x_new;
		y_cur=y_new;

		fourugb[i][j].type0=type;
		if ((i==is) &&(j==js))
		break;

	}
}
 //imshow("img",img);
 //cout<<"\n"<<counter;



 polygons[index]=counter;
 index++;
 int index_prev=index-1;
  cout<<"\n"<<" POLYGONS"<<counter3<<"\n\n\n\n";
 if (index!=0)
 if (polygons[index]!=polygons[index_prev])
	 goto abc;
 else 
	 for(int i=0;i<sizeof(polygons);i++)
		 cout<<polygons[i]<<"    ";///////////////////////////////check...error

xyz: imshow("img",img);
 g=g+2;
 myfile.close();

 //************************************* FEATURES******************************************************************************
 int p=0;
 eulerno(g,img2,&*fourugb);
 for (int i=0;i<counter3;i++) 
 {
	 if (box[p].boundary_no  != polygon[p].parent_poly  )
	 {cout<<"turuuu"<<box[p].boundary_no ;
	 polygon[p].hole=1;
	 polygon[p].euler=-10;}
	 p++;
 }
 concavity(g,img2,&*fourugb,counter3);
 convexity(g,img2,&*fourugb,counter3);
 hole_position(img,g,counter);
 VertHoriPerimeterComp(counter3);
 VertDirectionChange(counter3);
 imshow("img",img);
 
//*****************************************************CHECKKKK*****************************************************************
  p=0;
	/*for (iter = box.begin(); iter != box.end(); ++iter) 
	{
		cout<<"\n"<<p+1<<".\n";
		//cout<<"topx "<<box[p].topx<<"\n";
		//cout<<"topy "<<box[p].topy<<"\n";
		//cout<<"m "<<box[p].m<<"\n";
		cout<<"boundary number "<<box[p].boundary_no<<"\n";
		cout<<"parent poly"<<polygon[p].parent_poly<<"\n";
		//cout<<"containment_holes "<<box[p].containment_holes[0]<<"\n\n";
		p++;
	}*/
 p=0;
 //cout<<sizeof(polygon);
 /*for(int i=0;i<sizeof(polygon);i++)//.......................................................needs to be checked
	 cout<<polygon[i].hole ;*/

p=0;
	/*for (int i=0;i<counter3;i++) 
	{
		cout<<"\n"<<p+1<<".\n";
		cout<<"polygon[i].hole "<<polygon[i].hole<<"\n";
		cout<<"parent poly"<<polygon[p].parent_poly<<"\n";
		cout<<"euler "<<polygon[p].euler<<"\n";
		p++;
	}*/

//******************************************************************************************************************************



    cvWaitKey();
	getch();
	
	return 0;
}
