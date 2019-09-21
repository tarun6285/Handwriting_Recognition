#include<stdio.h>
#include<math.h>
#include<cv.h>
#include<highgui.h>

long double fact(long double n){
	if(n==1||n==0)
		return 1;
	else
		return n * (fact(n - 1.0));
}

long double comb(long double n, long double r){
	long double ans;
	ans = (fact(n))/(fact(r) * fact(n - r));
	return ans;
}

long double w(int x, double p, int N){
	long double ans = 0.0;
	ans = comb(N, x) * pow(p, x) * pow(1.0 - p, N - x);
	return ans;
}

long double poch(long double a, long double k){
	long double ans = 1.00;
	int i;
	for(i=0;i<k;i++){
		ans = ans * (a + i);
	}
	return ans;
}

long double rho(int n, long double p, int N){
	long double ans = 0.0, temp = 0.0;
	temp = pow(((1.0 - p) / p), n);
	ans = (pow(-1, n) * temp * fact(n))/(poch(-N, n));
	return ans;
}

long double mod(long double n){
	if(n>=0)
		return n;
	else
		return (n * (-1.0));
}

long double k(int n, int x, long double p, int N){
	long double ans = 0.0, temp, k;
	k = 0.00;
	temp = 0.00;
	do{
		temp = (poch(-n, k) * poch(-x, k) * pow((1.0 / p), k))/(poch(-N, k) * fact(k));
		ans = ans + temp;
		k = k + 1.0;
	}while(mod(temp) > 0.00000000000005);
	return ans;
}

long double kn(int n, int x, long double p, int N){
		long double ans, temp;
		ans = k(n, x, p, N);
		temp = w(x, p, N)/rho(n, p, N);
		ans = ans * pow(temp, 0.5);
		return ans;
}

int main(){
	FILE *fp, *fp1;
	char image_path[130], tag[10], name[50], moment[50];
	int i, j, k, l, h, w;
	long double p, ans, pix;
	printf("ENTER THE FILE NAME IN WHICH PATH OF ALL IMAGES IS WRITTEN ");
	scanf("%s",name);
	printf("ENTER THE VALUE OF P ");
	scanf("%d",&k);
	printf("ENTER THE VALUE OF Q ");
	scanf("%d",&l);
	sprintf(moment, "Moment_with_p_=_%d_and_q_=_%d.dat", k, l);
	fp = fopen(moment, "w");
	fp1 = fopen(name, "r");
	IplImage *src;
	CvScalar s;
	p = 0.5;
	while(fscanf(fp1,"%s",image_path) != EOF){
		src = cvLoadImage(image_path, 0);
		i=0;
		while(image_path[i] != '.'){
			i++;
		}
		i--;
		while(image_path[i] != '\\'){
			i--;
		}
		i--;
		while(image_path[i] != '\\'){
			i--;
		}
		i++;
		j=0;
		while(image_path[i] != '\\'){
			tag[j] = image_path[i];
			j++;
			i++;
		}
		tag[j] = '\0';
		h = src->height;
		w = src->width;
		ans = 0.0;
		for(i=0;i<h;i++){
			for(j=0;j<w;j++){
				s = cvGet2D(src, i, j);
				pix = s.val[0];
				ans = ans + (kn(k, i, p, h - 1) * kn(l, j, p, w - 1) * pix);
			}
		}
		fprintf(fp, "%lf\t", ans);
	    fflush(stdin);
		printf("done");
		fprintf(fp, "%s\n", tag);
	}
	fclose(fp);
	fclose(fp1);
	return 0;
}
