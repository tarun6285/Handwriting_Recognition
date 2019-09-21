#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cv.h>
#include <highgui.h>

char res[20];

FILE *fp, *fp1;

void main(int argc, char *argv[]) {
	int size, h, w;
    
	char image_path[30], name[30], moment[30];

    IplImage *src, *dest;

    int nmax, len, i, j, n, m;
	long double **f;
	nmax=20;
	
	printf("ENTER THE FILE NAME IN WHICH PATH OF ALL IMAGES IS WRITTEN ");
	scanf("%s",name);
	printf("ENTER THE VALUE OF P ");
	scanf("%d",&n);
	printf("ENTER THE VALUE OF Q LESS THAN OR EQUAL TO P WITH DIFFERENCE OF MULTIPLE OF 2 ");
	scanf("%d",&m);
	sprintf(moment, "Moment_with_p_=_%d_and_q_=_%d.dat", n, m);

    fp1 = fopen(name, "r");
    fp = fopen(moment, "w");

        f = (long double **) malloc(sizeof (long double *) *((501) / 2));
        for (i = 0; i <= (501) / 2; i++) {
            f[i] = (long double *) malloc(sizeof (long double) *(4 * (501)));
            for (j = 0; j <= 501; j++) {
                f[i][j] = 0;
            }
        }

        long double **c, **s1, factor, ***R;
        c = (long double **) malloc(sizeof (long double *) *(nmax));
        s1 = (long double **) malloc(sizeof (long double *) *(nmax));
        R = (long double ***) malloc(sizeof (long double **) *(nmax));
        for (i = 0; i <= nmax; i++) {
            c[i] = (long double *) malloc(sizeof (long double) *(nmax));
            s1[i] = (long double *) malloc(sizeof (long double) *(nmax));
            R[i] = (long double **) malloc(sizeof (long double *) *(nmax));
            for (j = 0; j <= nmax; j++) {
                c[i][j] = 0;
                s1[i][j] = 0;
	            R[i][j] = (long double *) malloc(sizeof (long double) *(500));
            }
        }

    while ((fscanf(fp1, "%s", image_path)) != EOF) {
        len = strlen(image_path);
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
			h = size;
			w = size;
		}else{
			size = w;
			if(size%2==0)
				size++;
			h = size;
			w = size;
		}

        dest = cvCreateImage(cvSize(size, size), IPL_DEPTH_8U, 1);
        CvScalar s;

        int sigma, rho, j1, i1;
        long double h1, h2, h3, r, p, theta, error = 0;
        cvResize(src, dest, 1);
		j=0;

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
		printf("hello");

            for (rho = 1; rho <= h / 2; rho++) {
                r = (long double) ((2.0 * rho) / h);
                R[n][n][rho] = pow(r, n);
                if ((n - 2) >= 0) {
                    R[n - 2][n - 2][rho] = pow(r, n - 2);
                    R[n][n - 2][rho] = (n * (R[n][n][rho]))-((n - 1) * (R[n - 2][n - 2][rho]));
                    for (j = n; j >= 4; j = j - 2) {
                        h3 = -(4.0 * (j - 2.0)*(j - 3.0)) / ((n + j - 2.0)*(n - j + 4.0));
                        h2 = ((h3 * (n + j)*(n - j + 2.0)) / (4.0 * (j - 1.0)))+(j - 2.0);
                        h1 = ((j * (j - 1.0)) / 2.0)-(j * h2)+((h3 * (n + j + 2.0)*(n - j)) / 8.0);
                        R[n][j - 4][rho] = h1 * R[n][j][rho]+(h2 + (h3 / (r * r))) * R[n][j - 2][rho];
                    }
                }
                for (sigma = 1; sigma <= (8 * rho); sigma++) {
                    theta = ((3.141593) * sigma) / (4.0 * rho);
                    if ((n - m) % 2 == 0) {
                        c[n][m] = c[n][m] + ((R[n][m][rho]) * (cos(m * theta)) * (f[rho][sigma]));
                        s1[n][m] = s1[n][m] + ((R[n][m][rho]) * (sin(m * theta))* (f[rho][sigma]));
                    }
                }
            }

            factor = (long double) ((2.0 * n + 2.0) / (h*h));
        if ((n - m) % 2 == 0) {
            c[n][m] = c[n][m] * factor;
            s1[n][m] = s1[n][m] * factor;
            fprintf(fp, "%f\t", c[n][m]);
            fprintf(fp, "%f\t", s1[n][m]);
            fflush(stdin);
			fprintf(fp, "%s\n", res);
			fflush(stdin);
        }
	}
    fclose(fp);
    fclose(fp1);
}
