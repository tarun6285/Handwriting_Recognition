#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <direct.h>

int N;
char res[20];
CvScalar s;
IplImage *dest;

double rec(int p, int x){
	if(p == 0)
		return 1;
	else if(p == 1)
		return ((2.0 * x - N + 1.0)/N);
	else{
		double a, b;
		a = (2.0 * p - 1.0)*(rec(1, x))*(rec(p-1, x));
		b = (1.0 - ((pow(p-1,2))/(pow(N,2))));
		b = (p - 1.0)*b*rec(p-2, x);
		return ((a-b)/p);
	}	
}

double rho(int p, int m){
	double a;
	int i;
	a = m;
	for(i=1;i<=p;i++){
		a = a*(1.0 - ((pow(i,2))/(pow(m,2))));
	}
	a = a/(2.0 * p + 1.0);
	return a;
}

double moment(int p, int q){
	int i, j;
	double pix, sum = 0;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			s = cvGet2D(dest, i, j);
			pix = s.val[0];
			sum = sum + (rec(p, i)*rec(q, j)*pix);
		}
	}
	sum = sum/((rho(p, N))*(rho(q, N)));
	return sum;
}

void main(int argc, char *argv[]) {
	FILE *fp, *fp1;
    char* image_path = (char*) malloc(sizeof (char) *100); // argv[1] contains name of image file with path
    IplImage *src, *final;
	char name[50], moment[50];
	int h, w, size, i, j, k, l, len;
	double pix, sum;
	printf("ENTER THE FILE NAME IN WHICH PATH OF ALL IMAGES IS WRITTEN ");
	scanf("%s",name);
	printf("ENTER THE VALUE OF P ");
	scanf("%d",&k);
	printf("ENTER THE VALUE OF Q ");
	scanf("%d",&l);
	sprintf(moment, "Moment_with_p_=_%d_and_q_=_%d.dat", k, l);
	fp = fopen(moment,"w");
	fp1 = fopen(name, "r");
    while ((fscanf(fp1, "%s", image_path)) != EOF) {
		j=0;
		src = cvLoadImage(image_path, 0);
        len = strlen(image_path);
        i = len;
        while (image_path[i] != '\\') {
            i--;
        }
		i--;
        while (image_path[i] != '\\') {
            i--;
        }
        i++;
        while (image_path[i] != '\\') {
            res[j] = image_path[i];
            i++;
            j++;
        }
		res[j]='\0';
		h = src->height;
		w = src->width;
		if(h<w){
			size = h;
			w = h;
			N = h;
		}else{
			size = w;
			h = w;
			N = w;
		}
		dest = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 1);
		final = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 1);
		cvResize(src, dest, 1);
		sum = 0;
		for(i=0;i<N;i++){
			for(j=0;j<N;j++){
				s = cvGet2D(dest, i, j);
				pix = s.val[0];
				sum = sum + (rec(k, i) * rec(l, j) * pix);
			}
		}
		sum = sum/((rho(k, N))*(rho(l, N)));
		fprintf(fp,"%lf\t", sum);
		fprintf(fp,"%s\n", res);
	}
	fclose(fp);
}
