#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

char res[20], moment[30], name[30];

FILE *fp, *fp1;

void main(int argc, char *argv[]) {
    char* image_path = (char*) malloc(sizeof(char) *100); //argv[1] contains name of image file with path

    IplImage *src, *dest, *new1;

    int n, m, nmax, len, i4, i, j;
	
    nmax=40;
    long double **c, **s1, factor, ***R, **f;
    c = (long double **) malloc(sizeof (long double *) *(nmax));
    s1 = (long double **) malloc(sizeof (long double *) *(nmax));
    R = (long double ***) malloc(sizeof (long double **) *(nmax));
    for (i = 0; i <= nmax; i++) {
	    c[i] = (long double *) malloc(sizeof (long double) *(nmax));
		s1[i] = (long double *) malloc(sizeof (long double) *(nmax));
        R[i] = (long double **) malloc(sizeof (long double *) *(nmax));
        for (j = 0; j <= nmax; j++) {
			c[i][j]=0;
			s1[i][j]=0;
		    R[i][j] = (long double *) malloc(sizeof (long double) *(700));
        }
    }
	
    f = (long double **) malloc(sizeof (long double *) *((500) / 2));
    for (i = 0; i <= (500) / 2; i++) {
        f[i] = (long double *) malloc(sizeof (long double) *(500 * (500 + 1)));
        for (j = 0; j <= 500 + 1; j++) {
            f[i][j] = 0;
        }
    }

	printf("ENTER THE FILE NAME IN WHICH PATH OF ALL IMAGES IS WRITTEN ");
	scanf("%s",name);
	printf("ENTER THE VALUE OF P ");
	scanf("%d",&n);
	printf("ENTER THE VALUE OF Q LESS THAN OR EQUAL TO P");
	scanf("%d",&m);
	sprintf(moment, "Moment_with_p_=_%d_and_q_=_%d.dat", n, m);

    fp1 = fopen(name, "r");
    fp = fopen(moment, "w");
    int size, h, w;
	while ((fscanf(fp1, "%s", image_path)) != EOF) {
        len = strlen(image_path);
        i4 = len;
        if (image_path != NULL)
            src = cvLoadImage(image_path, 0); //src: load the source image
        else
            return;

        if (src == NULL) return;
		h = src->height;
		w = src->width;
		if(h<w){
			size = h;
			if(size%2==0)
				size++;
		}else{
			size = w;
			if(size%2==0)
				size++;
		}

        dest = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 1);
        new1 = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 1);
        CvScalar s;

        int j = 0, sigma, rho, j1, i1;
        long double r, p, theta, error = 0, l1, l2, l3;
        cvResize(src, dest, 1);

        h = dest->height;
        w = dest->width;
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

        s = cvGet2D(dest, h / 2, h / 2);
        p = s.val[0];
        f[0][0] = p;
        rho = 1;
        j1 = 1;
        i1 = 1;
        while (rho <= h / 2) {
            sigma = 1;
            i = i1;
            j = j1;
            s = cvGet2D(dest, h / 2 + i, j + h / 2);
            p = s.val[0];
            f[rho][sigma] = p;
            while (sigma < ((2 * rho) + 1)) {
                i--;
                sigma++;
                s = cvGet2D(dest, h / 2 + i, j + h / 2);
                p = s.val[0];
                f[rho][sigma] = p;
            }
            while (sigma < ((4 * rho) + 1)) {
                j--;
                sigma++;
                s = cvGet2D(dest, h / 2 + i, j + h / 2);
                p = s.val[0];
                f[rho][sigma] = p;
            }
            while (sigma < ((6 * rho) + 1)) {
                i++;
                sigma++;
                s = cvGet2D(dest, h / 2 + i, j + h / 2);
                p = s.val[0];
                f[rho][sigma] = p;
            }
            while (sigma < (8 * rho)) {
                j++;
                sigma++;
                s = cvGet2D(dest, h / 2 + i, j + h / 2);
                p = s.val[0];
                f[rho][sigma] = p;
            }
            rho++;
            j1++;
            i1++;
        }
            for (rho = 1; rho <= h / 2; rho++) {
                r = (long double) ((2.0 * rho) / h);
                R[n][n][rho] = pow(r, n);
				if(n>0){
					R[n][n-1][rho] = (long double)((2.0 * n) + 1.0) * R[n][n][rho] - (2.0 * n) * (pow(r, n-1));
				}
				for(j=0;j<n-1;j++){
					l1 = (long double)(((2.0 * n) + 1.0) * (2.0 * n)) / ((n + j + 1.0) * (n - j));
					l2 = (long double)((-2.0) * n) + (((n + j) * (n - j - 1.0)) / (2.0 * n - 1.0)) * l1;
					l3 = (long double)((2.0 * n - 1.0) * (n - 1.0)) - (((n + j - 1.0) * (n - j - 2.0) / 2.0) * l1) + 2.0 * (n - 1.0) * l2;
					R[n][j][rho] = ((l1 * (pow(r,1))) + l2) * (R[n - 1][j][rho]) + l3 * R[n - 2][j][rho];
				}
                for (sigma = 1; sigma <= (8 * rho); sigma++) {
                    theta = ((3.141593) * sigma) / (4.0 * rho);
                    c[n][m] = c[n][m] + ((R[n][m][rho]) * (cos(m * theta)) * (f[rho][sigma]));
                    s1[n][m] = s1[n][m] + ((R[n][m][rho]) * (sin(m * theta))* (f[rho][sigma]));
                }
            }

            factor = (long double) ((2.0 * n + 2.0) / (h*h));
            c[n][m] = c[n][m] * factor;
            s1[n][m] = s1[n][m] * factor;
            fprintf(fp, "%lf\t", c[n][m]);
            fprintf(fp, "%lf\t", s1[n][m]);
            fflush(stdin);
        fprintf(fp, "%s\n", res);
    }
    fclose(fp);
    fclose(fp1);
}
