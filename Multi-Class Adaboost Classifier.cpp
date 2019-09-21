#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<string.h>

#define train_size 3783  // No. of samples in the training set for adaboost (i.e., the validation set)
#define test_size 3763   // No. of samples in the test set
#define no_of_train 4    // No. of classifiers to combine
#define no_of_test 4    // No. of classifiers to combine
#define starting_class_no 0  // starting_class_no is the starting class number which is defined to avoid the problems
                            // when we shift from set which has starting class number a to any other class number b
#define last_class_no 9 // largest valued class for eg in our case from 0 to 9, 9 is maximum
#define WEIGHT_ADJUSTMENT_FACTOR 0.16666667 // the value is equal to 6; this may be changed and see the performance

double error_weight[no_of_train+1][no_of_train+1]; //error_weight[i][j] stores the sum of the weights (ith iteration) of wrongly classified samples by jth classifier
double train_data[no_of_train+1][train_size+1]; // train_data[i][j] stores the data from training file with ith classifier and jth sample from training set
double new_train_data[no_of_train+1][train_size+1]; // new_train_data[i][j] will be changed each time adaboost function is called
													// when adaboost function is called with class value as argument then jth class sample
													// corresponding to that class of ith training classifier will be set to 1 and others are set to -1
double test_data[no_of_train+1][test_size+1]; // test_data[i][j] stores the data from testing file with ith classifier and jth sample from testing set
double new_test_data[no_of_train+1][test_size+1];// new_test_data[i][j] will be changed each time adaboost function is called
												 // when adaboost function is called with class value as argument then jth class sample
												 // corresponding to that class of ith testing classifier will be set to 1 and others are set to -1
double total_weight[no_of_train+1][no_of_train+1]; // total_weight[i][j] stores the sum of the weights (ith iteration) of all samples by jth classifier
int flag[no_of_train+1];// flag[i] is declared to check whether the ith classifier which has been chosen is not going to be chosen again
int n;// n is the number of classifiers chosen to be mixed so as to produce a strong classifier
int final[test_size+1];// flag[i] contains the value 1 for all i but as soon as i1 classifiers is chosen flag[i1] is set to 0 so as differentiate between chosen
					   // and not chosen classifier
int min_error_classifier[no_of_train+1];// min_error_classifier[i] contains the classifier with minimum error in ith iteration
int conf_matrix[last_class_no+1][last_class_no+1];// conf_matrix[i][j] contains value which determine that how many samples of ith class are recoganized as j
double weight[no_of_train+1][train_size+1];// weight[i][j] contains the weights of jth sample in ith iteration which is updated in each iteration
double approx[test_size+1];// approx[i] contains the approximate class value for each sample by determining its sign we will determine particular sample class
double error[no_of_train+1][no_of_train+1];// error[i][j] contains the error for jth classifier in ith iteration and it will be updated in each iteration
										   // because it is ratio of error weight and total weight and error weight will be updated in each iteration so
										   // this also will be updated in each iteration.
double alfa[no_of_train+1]; // alfa[i] is the weight assigned to the ith classifier at the end
	
void adaboost(int);// adaboost(i) is adaboost function which is called each time i(starting_class_no to last_class_no) changes that means all class values are
                   // passed as argument into function

int min(int i) { //min(i) function finds the classifier with minimum weighted error 
    int j, min1;
    double min = 9999;
    for (j = 1; j <= no_of_train; j++) {
        if ((min > error_weight[i][j]) && (flag[j] == 1)) {
            min = error_weight[i][j];
            min1 = j;
        }
    }
    flag[min1] = 0; //It does not allow a classifier chosen before to be selected again during another iteration
	                // To allow a classifier to be selected repeatedly at different iterations this flag should be assigned 1
    return min1;
}

void main() {
    int  x;
	char str[50]; //To store file names
	printf("ENTER NO. OF CLASSIFIERS "); // It is the number of classifiers to be combined; 
	                                     //This is always <= no_of_train (no. of trained classifiers); 
	                                     //If n < no_of_train, then it picks up only the best subset of classifiers
    scanf("%d", &n);
    FILE *fp, *fp1, *fp2;
	fp=fopen("inp.txt","r"); //It contains the list of complete paths of all the input files
	                         //Input files consist of all testing files, validation files and true class tags of validation samples
	                         //The order of file names in the list should strictly follow the above order
	                         //These files contain either the classifier generated class tag ot the true class tag
	fscanf(fp,"%s",str);
    int i=1,j;
	fp1=fopen(str,"r");
    while(fp1!=0x00000000){ //checking of end-of-file
		if(i<=no_of_test){
		    for(j=1;j<=test_size;j++){
			    fscanf(fp1, "%d", &x);
				test_data[i][j]=x;
			}
		}else{
			if(i>(no_of_test + no_of_train)){
			    for(j=1;j<=train_size;j++){
				    fscanf(fp1, "%d", &x);
					train_data[0][j]=x;
				}
			}else{
			    for(j=1;j<=train_size;j++){
				    fscanf(fp1, "%d", &x);
					train_data[i-no_of_test][j]=x;
				}
			}
		}
		i++;
		fscanf(fp,"%s",str);
		fp1=fopen(str,"r");
	}

	fp1 = fopen("adaboost.txt", "w+"); //stores the final output on which match.exe should be run to get the confusion matrix.
    
    for(i=starting_class_no;i<=last_class_no;i++)
        adaboost(i);
    
	i = 1;
    while (i <= test_size) {
        fprintf(fp1, "%d\n", final[i]);
        fflush(stdin);
        i++;
    }
	
    fclose(fp);
    rewind(fp1);
    fp=fopen("confusion_matrix.txt","w");
    fp2 = fopen("output.txt", "r"); //stores the final output on which match.exe should be run to get the confusion matrix.
    for(i=starting_class_no;i<=last_class_no;i++){
        for(j=starting_class_no;j<=last_class_no;j++){
            conf_matrix[i][j]=0;
        }
    }
    char str1[50], str2[50];// to store the data from adabosst file and correct testing file
    int a,b,total=0;
    double acc;// contains the accuracy of adaboost
    while(fscanf(fp1,"%s",str1)!=-1){
		fscanf(fp2,"%s",str2);
        a=atoi(str1);
        b=atoi(str2);
        conf_matrix[b][a]++;
    }

    for(i=starting_class_no;i<=last_class_no;i++){
        for(j=starting_class_no;j<=last_class_no;j++){
            fprintf(fp,"%d\t",conf_matrix[i][j]);
        }
        fprintf(fp,"\n");
    }

    for(i=starting_class_no;i<=last_class_no;i++){
        total = total + conf_matrix[i][i];
    }

    acc=(double)total/(double)test_size;
    fprintf(fp,"\nACCURACY IS %lf",acc*100);
    fclose(fp);
    fclose(fp1);
    fclose(fp2);
	printf("\ndone...");
}

void adaboost(int classval){
	int i, j, l;
	for (i = 1; i <= no_of_train; i++)
        flag[i] = 1;
    for (i = 1; i <= test_size; i++)
        approx[i] = 0;
    for (l = 1; l <= train_size; l++)
        weight[1][l] = 1;
	
	for(i=0;i<=no_of_train;i++){
        for(j=1;j<=train_size;j++){
            if(train_data[i][j]==classval)
                new_train_data[i][j]=1;
            else
                new_train_data[i][j]=-1;
        }
    }
	
	for(i=1;i<=no_of_test;i++){
        for(j=1;j<=test_size;j++){
            if(test_data[i][j]==classval)
                new_test_data[i][j]=1;
            else
                new_test_data[i][j]=-1;
        }
    }

    for (i = 1; i <= n; i++) {
        for (j = 1; j <= no_of_train; j++) {
            for (l = 1; l <= train_size; l++) {
                if (l == 1) {
                    error_weight[i][j] = 0;
                    total_weight[i][j] = 0;
	            }
                if(new_train_data[0][l] != new_train_data[j][l])
                    error_weight[i][j] = error_weight[i][j] + weight[i][l];
				    
                total_weight[i][j] = total_weight[i][j] + weight[i][l];
            }
        }
		min_error_classifier[i] = min(i);  //it stores the best classifier of the ith iteration with minimum no. of erroneous recognition during 
		error[i][min_error_classifier[i]] = error_weight[i][min_error_classifier[i]] / total_weight[i][min_error_classifier[i]];
	    alfa[i] = log((1.0 - error[i][min_error_classifier[i]]) / error[i][min_error_classifier[i]]); // it stores the weight of the best classifier of the ith iteration
		alfa[i]=alfa[i]*WEIGHT_ADJUSTMENT_FACTOR;
		if(alfa[i]<0)
			alfa[i]=0;
	    printf("%d %f\n", min_error_classifier[i], alfa[i]);
		if (i == n)
		    break;
                
		for (l = 1; l <= train_size; l++) {
		    double t = ((1.0 - error[i][min_error_classifier[i]]) / (error[i][min_error_classifier[i]]));
		    if (new_train_data[0][l] != new_train_data[min_error_classifier[i]][l])
		        weight[i + 1][l] = weight[i][l]*(pow(t, WEIGHT_ADJUSTMENT_FACTOR));
		    else
		        weight[i + 1][l] = weight[i][l]*(pow(t, -WEIGHT_ADJUSTMENT_FACTOR));
		}
	}

    for (i = 1; i <= n; i++) {
        for (j = 1; j <= test_size; j++) {
            approx[j] = approx[j]+((new_test_data[min_error_classifier[i]][j]) * (alfa[i]));
        }
    }
	
	printf("\n");

	for(i=1;i<=test_size;i++){
        if(approx[i]>0)
            final[i]=classval;
    }
}
