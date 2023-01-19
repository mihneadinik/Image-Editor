//Dinica Mihnea-Gabriel 313CA
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 100000

//function that transforms a string into a number
int char_to_int(char num[])
{
	int i, nr = 0, sz = strlen(num), dig;
	for (i = 0; i < sz; i++) {
		dig = num[i] - '0';
		nr = nr * 10 + dig;
	}
	return nr;
}

//function that deallocates a matrix
void free_matrix(int n, int **mat)
{
	int i;
	for (i = 0; i < n; i++)
		free(mat[i]);
	free(mat);
}

//function that checks if a file has been opened successfully
int check_file(char *command, char *file_return)
{
	char *file, command_cp[100];
	FILE *picture;
	strcpy(command_cp, command);
	file = strtok(command_cp, " ");
	file = strtok(NULL, " ");
	strcat(file, "\0");
	strcpy(file_return, file);
	picture = fopen(file, "rb");
	if (!picture)
		return 0;
	fclose(picture);
	return 1;
}

//function that creates a string number
void number_create(int ch1, int ch2, int ch3,
				   char c1[2], char c2[2], char c3[2], char num_end[4])
{
	num_end[0] = '\0';
	if (ch1 == 1)
		strcat(num_end, c1), num_end[1] = '\0';
	if (ch2 == 1)
		strcat(num_end, c2), num_end[2] = '\0';
	if (ch3 == 1)
		strcat(num_end, c3), num_end[3] = '\0';
}

//function that reads a matrix from an ascii file
int **read_mat(int **mat, int file_poz, char *file, int x, int y)
{
	FILE *picture;
	int ch1, ch2, ch3, pix, bx = 0, by = 0;
	char num_end[4] = "\0", c1[2] = "\0";
	char c2[2] = "\0", c3[2] = "\0", num[2] = "\0";
	picture = fopen(file, "rb");
	fseek(picture, file_poz, SEEK_SET);
	while (!feof(picture)) {
		//at every loop we reinitialise these variables
		//that keep track of how many digits a number has
		ch1 = 0, ch2 = 0, ch3 = 0;
		//reinitialising the strings with nothing inside
		//num_end is the final number and c1-c3 are its digits
		num_end[0] = '\0', c1[0] = '\0', c2[0] = '\0', c3[0] = '\0';
		fread(num, 1, sizeof(char), picture);
		//reading every digit between blanks, new lines and tabs
		if (num[0] == ' ' || num[0] == '\n' || num[0] == '\t') {
			fread(c1, 1, sizeof(char), picture), c1[1] = '\0';
			if (c1[0] != ' ' && c1[0] != '\n' && c1[0] != '\t') {
				ch1 = 1;
				fread(c2, 1, sizeof(char), picture), c2[1] = '\0';
				if (c2[0] != ' ' && c2[0] != '\n' && c2[0] != '\t') {
					ch2 = 1;
					fread(c3, 1, sizeof(char), picture), c3[1] = '\0';
					if (c3[0] != ' ' && c3[0] != '\n' && c3[0] != '\t')
						ch3 = 1;
				}
			}
		} else {
			fseek(picture, -2, SEEK_CUR);
		}
		if (ch1 == 0)
			continue;
		if (ch1 == 1 && ch2 == 0)
			fseek(picture, -1, SEEK_CUR);
		if (ch2 == 1 && ch3 == 0)
			fseek(picture, -1, SEEK_CUR);
		number_create(ch1, ch2, ch3, c1, c2, c3, num_end);
		//adding it to the matrix
		pix = char_to_int(num_end);
		if (bx == x)
			bx = 0, by++;
		if (by == y)
			break;
		mat[by][bx++] = pix;
	}
	fclose(picture);
	return mat;
}

//LOAD function
int **load_fct(char *command, char *type, int *xs, int *ys, char *maxpix)
{
	FILE *picture;
	char *file, *p, dimx[NMAX] = "\0", dimy[NMAX] = "\0";
	char line[100];
	int x, y, bin = 0;
	//getting filename
	file = strtok(command, " "), file = strtok(NULL, " ");
	picture = fopen(file, "rb");
	printf("Loaded %s\n", file);
	//ignoring if commentary
	while (fgets(line, 100, picture) && (line[0] == '#'))
		continue;
	//saving image type
	line[strlen(line) - 1] = '\0', strcpy(type, line);
	if (type[1] == '4' || type[1] == '5' || type[1] == '6')
		bin = 1;
	//ignoring if commentary
	while (fgets(line, 100, picture) && (line[0] == '#'))
		continue;
	//getting dimensions
	line[strlen(line) - 1] = '\0';
	p = strtok(line, " "), strcpy(dimx, p);
	p = strtok(NULL, " "), strcpy(dimy, p);
	//ignoring if commentary
	while (fgets(line, 100, picture) && (line[0] == '#'))
		continue;
	if (strcmp(type, "P1") != 0 && strcmp(type, "P4") != 0) {
		//getting max pixel value
		line[strlen(line) - 1] = '\0';
		strcpy(maxpix, line);
		//ignoring if commentary
		while (fgets(line, 100, picture) && (line[0] == '#'))
			continue;
	}
	//going back one line
	int mv_back = strlen(line) + 1;
	fseek(picture, -mv_back, SEEK_CUR);
	x = char_to_int(dimx), y = char_to_int(dimy);
	//creating the matrix
	if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
		x = x * 3;
	int **mat;
	//reading the matrix char by char until the end
	if (bin == 1) {
		fseek(picture, 1, SEEK_CUR);
		unsigned char read_cur;
		mat = (int **)calloc(y, sizeof(int *));
		for (int i = 0; i < y; i++) {
			mat[i] = (int *)calloc(x, sizeof(int));
			for (int j = 0; j < x; j++) {
				fread(&read_cur, 1, sizeof(unsigned char), picture);
				mat[i][j] = read_cur;
			}
		}
	} else {
		mat = (int **)calloc(y, sizeof(int *));
		for (int i = 0; i < y; i++)
			mat[i] = (int *)calloc(x, sizeof(int));
		int file_poz = ftell(picture);
		fclose(picture);
		mat = read_mat(mat, file_poz, file, x, y);
	}
	*xs = x, *ys = y;
	return mat;
}

//SAVE function
void save_fct(char *command, int **mat, int x,
			  int y, char maxpix[5], char type[3])
{
	FILE *save;
	char *file_name, *code;
	//getting the file name
	file_name = strtok(command, " ");
	file_name = strtok(NULL, " ");
	//checking if we have "ascii"
	code = strtok(NULL, " ");
	if (code) {
		save = fopen(file_name, "w");
		if (strcmp(type, "P4") == 0)
			strcpy(type, "P1");
		if (strcmp(type, "P5") == 0)
			strcpy(type, "P2");
		if (strcmp(type, "P6") == 0)
			strcpy(type, "P3");
		if (strcmp(type, "P3") == 0)
			fprintf(save, "%s\n%d %d\n%s\n", type, x / 3, y, maxpix);
		else
			fprintf(save, "%s\n%d %d\n%s\n", type, x, y, maxpix);
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++)
				fprintf(save, "%d ", mat[i][j]);
			fprintf(save, "\n");
		}
	} else {
		save = fopen(file_name, "wb");
		if (strcmp(type, "P1") == 0)
			strcpy(type, "P4");
		if (strcmp(type, "P2") == 0)
			strcpy(type, "P5");
		if (strcmp(type, "P3") == 0)
			strcpy(type, "P6");
		if (strcmp(type, "P6") == 0)
			fprintf(save, "%s\n%d %d\n%s\n", type, x / 3, y, maxpix);
		else
			fprintf(save, "%s\n%d %d\n%s\n", type, x, y, maxpix);
		unsigned char print_val;
		for (int i = 0; i < y; i++) {
			for (int j = 0; j < x; j++) {
				print_val = (unsigned char)mat[i][j];
				fwrite(&print_val, 1, sizeof(unsigned char), save);
			}
		}
	}
	fclose(save);
	printf("Saved %s\n", file_name);
}

//function that sorts 2 integers
void compare(int *a, int *b)
{
	if (*a > *b) {
		int aux = *a;
		*a = *b;
		*b = aux;
	}
}

//function that checks if a pixel is bigger than the max value
int check_big(int maxval, int pix)
{
	if (pix > maxval)
		return maxval;
	else
		return pix;
}

//function that rounds a number
int roundd(double a)
{
	int q = a;
	if ((a - q) >= (q + 1 - a))
		return q + 1;
	else
		return q;
}

//function that verifies if a selection is valid
int check_select(char *command)
{
	int sz;
	char *w1, command_cp[100];
	strcpy(command_cp, command);
	w1 = strtok(command_cp, " ");
	w1 = strtok(NULL, " ");
	if (strcmp(w1, "ALL") == 0)
		return 1;
	//checking if arguments are numbers
	char *w2, *w3, *w4;
	w2 = strtok(NULL, " ");
	if (!w2)
		return 0;
	sz = strlen(w2);
	for (int i = 0; i < sz; i++) {
		if (w2[i] == '-')
			continue;
		if (w2[i] < '0' || w2[i] > '9')
			return 0;
	}
	w3 = strtok(NULL, " ");
	if (!w3)
		return 0;
	sz = strlen(w3);
	for (int i = 0; i < sz; i++) {
		if (w3[i] == '-')
			continue;
		if (w3[i] < '0' || w3[i] > '9')
			return 0;
	}
	w4 = strtok(NULL, " ");
	if (!w4)
		return 0;
	sz = strlen(w4);
	for (int i = 0; i < sz; i++) {
		if (w4[i] == '-')
			continue;
		if (w4[i] < '0' || w4[i] > '9')
			return 0;
	}
	return 1;
}

//SELECT function
void select_fct(char *command, int x, int y,
				int *x1, int *y1, int *x2, int *y2, char type[3])
{
	char *w1;
	//getting the first atribute
	w1 = strtok(command, " ");
	w1 = strtok(NULL, " ");
	//checking the type of selection
	if (strcmp(w1, "ALL") == 0) {
		printf("Selected ALL\n");
		*x1 = 0;
		*y1 = 0;
		if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
			*x2 = x + 2;
		else
			*x2 = x;
		*y2 = y;
	} else {
		char *w2, *w3, *w4;
		w2 = strtok(NULL, " ");
		w3 = strtok(NULL, " ");
		w4 = strtok(NULL, " ");
		int xc1 = char_to_int(w1), yc1 = char_to_int(w2);
		int xc2 = char_to_int(w3), yc2 = char_to_int(w4);
		//checking if they are in the right order
		compare(&xc1, &xc2);
		compare(&yc1, &yc2);
		//checking if the image is color
		if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0) {
			xc1 *= 3;
			xc2 = xc2 * 3 + 2;
			x += 3;
		}
		//checking to see if they are inside the matrix's borders
		if (xc1 < 0 || xc2 < 0 || xc2 > x || yc1 < 0 ||
			yc1 < 0 || yc2 > y || xc1 == xc2 || yc1 == yc2) {
			printf("Invalid set of coordinates\n");
		} else {
			*x1 = xc1;
			*y1 = yc1;
			*x2 = xc2;
			*y2 = yc2;
			if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
				printf("Selected %d %d %d %d\n",
					   xc1 / 3, yc1, (xc2 - 2) / 3, yc2);
			else
				printf("Selected %d %d %d %d\n", xc1, yc1, xc2, yc2);
		}
	}
}

//CROP function
int **crop_fct(int *x, int *y, int x1, int y1,
			   int x2, int y2, int **mat, char type[3])
{
	if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
		x2 -= 2;
	x2--;
	y2--;
	//xn, yn are the new sizes
	int i, j, xn = x2 - x1 + 1, yn = y2 - y1 + 1, bx = 0, by = 0;
	//constructing a new matrix
	int **matn = (int **)calloc(yn, sizeof(int *));
	for (i = 0; i < yn; i++)
		matn[i] = (int *)calloc(xn, sizeof(int));
	//checking each element if it's allowed to be added to the mew matrix
	for (i = 0; i < *y; i++) {
		for (j = 0; j < *x; j++) {
			if (i >= y1 && i <= y2 && j >= x1 && j <= x2) {
				//adding to the new matrix
				if (bx == xn) {
					bx = 0;
					by++;
				}
				matn[by][bx] = mat[i][j];
				bx++;
			}
		}
	}
	//changing the matrices
	free_matrix(*y, mat);
	//mat = matn;
	*x = xn;
	*y = yn;
	return matn;
}

//GRAYSCALE function
int **grayscale_fct(int x1, int y1, int x2, int y2, int **mat)
{
	x2 -= 3;
	y2--;
	int i, j, sum, val;
	double pix;
	//calculating the new value for coloured pixels
	for (i = y1; i <= y2; i++) {
		for (j = x1; j <= x2; j += 3) {
			sum = mat[i][j] + mat[i][j + 1] + mat[i][j + 2];
			pix = (1.0 * sum) / 3;
			val = roundd(pix);
			mat[i][j] = val;
			mat[i][j + 1] = val;
			mat[i][j + 2] = val;
		}
	}
	return mat;
}

//SEPIA function
int **sepia_fct(int x1, int y1, int x2, int y2, int **mat, int mxpix)
{
	x2 -= 3;
	y2--;
	int i, j, r, g, b;
	double pr, pg, pb;
	//calculating the new value for coloured pixels
	for (i = y1; i <= y2; i++) {
		for (j = x1; j <= x2; j += 3) {
			r = mat[i][j];
			g = mat[i][j + 1];
			b = mat[i][j + 2];
			//applying the formula
			pr = 0.393 * r + 0.769 * g + 0.189 * b;
			pg = 0.349 * r + 0.686 * g + 0.168 * b;
			pb = 0.272 * r + 0.534 * g + 0.131 * b;
			//rounding the numbers
			r = roundd(pr);
			g = roundd(pg);
			b = roundd(pb);
			//adding to the matrix + checking the condition
			mat[i][j] = check_big(mxpix, r);
			mat[i][j + 1] = check_big(mxpix, g);
			mat[i][j + 2] = check_big(mxpix, b);
		}
	}
	return mat;
}

//function that rotates 90 to the right inside a noncolor image
int **right_rot_noncol(int x1, int y1, int x2, int y2, int **mat, int **matwork)
{
	int i, j, addy, addx = x1;
	for (i = y1; i <= y2; i++) {
		addy = 0;
		for (j = x1; j <= x2; j++) {
			matwork[i][j] = mat[y2 - addy][addx];
			addy++;
		}
		addx++;
	}
	for (i = y1; i <= y2; i++)
		for (j = x1; j <= x2; j++)
			mat[i][j] = matwork[i][j];
	return mat;
}

//function that rotates 90 to the right a full noncolor image
int **full_right_rot_noncol(int x, int y, int **mat, int **matwork)
{
	int i, j, bx = 0, by = 0;
	for (i = 0; i < x; i++) {
		for (j = y - 1; j >= 0; j--) {
			if (bx == y) {
				bx = 0;
				by++;
			}
			matwork[by][bx] =  mat[j][i];
			bx++;
		}
	}
	return matwork;
}

//function that rotates 90 to the left a full noncolor image
int **full_left_rot_noncol(int x, int y, int **mat, int **matwork)
{
	int i, j, bx = 0, by = 0;
	for (i = x - 1; i >= 0; i--) {
		for (j = 0; j < y; j++) {
			if (bx == y) {
				bx = 0;
				by++;
			}
			matwork[by][bx] =  mat[j][i];
			bx++;
		}
	}
	return matwork;
}

//function that rotates 90 to the right inside a color image
int **right_rot_col(int x1, int y1, int x2, int y2, int **mat, int **matwork)
{
	int i, j, addx = x1, addy;
	for (i = y1; i <= y2; i++) {
		addy = 0;
		for (j = x1 / 3; j <= (x2 - 2) / 3; j++) {
			matwork[i][j * 3] = mat[y2 - addy][addx];
			matwork[i][j * 3 + 1] = mat[y2 - addy][addx + 1];
			matwork[i][j * 3 + 2] = mat[y2 - addy][addx + 2];
			addy++;
		}
		addx += 3;
	}
	for (i = y1; i <= y2; i++)
		for (j = x1; j <= x2; j++)
			mat[i][j] = matwork[i][j];
	return mat;
}

//function that rotates 90 to the right a full color image
int **full_right_rot_col(int x, int y, int **mat, int **matwork)
{
	int i, j, bx = 0, by = 0, addx = 0;
	for (i = 0; i < x / 3; i++) {
		for (j = y - 1; j >= 0; j--) {
			if (bx == y * 3)
				bx = 0, by++;
			matwork[by][bx] =  mat[j][addx];
			matwork[by][bx + 1] =  mat[j][addx + 1];
			matwork[by][bx + 2] =  mat[j][addx + 2];
			bx += 3;
		}
		addx += 3;
	}
	return matwork;
}

//function that rotates 90 to the left a full color image
int **full_left_rot_col(int x, int y, int **mat, int **matwork)
{
	int i, j, bx = 0, by = 0;
	for (i = x / 3 - 1; i >= 0; i--) {
		for (j = 0; j < y; j++) {
			if (bx == y * 3)
				bx = 0, by++;
			matwork[by][bx] =  mat[j][i * 3];
			matwork[by][bx + 1] =  mat[j][i * 3 + 1];
			matwork[by][bx + 2] =  mat[j][i * 3 + 2];
			bx += 3;
		}
	}
	return matwork;
}

//function that prints some output from the ROTATE function
void print_rotate(int ang, int poz)
{
	if (poz == 1)
		printf("Rotated %d\n", ang);
	else
		printf("Rotated -%d\n", ang);
}

//function that saves the angle and its sign
int poz_or_neg(char *angle, char *command, int *ang)
{
	angle = strtok(command, " ");
	angle = strtok(NULL, " ");
	int len = strlen(angle), i;
	//checking if it's a positive rotation or not
	if (angle[0] == '-') {
		for (i = 0; i < len; i++)
			angle[i] = angle[i + 1];
		*ang = char_to_int(angle);
		return 0;
	}
	*ang = char_to_int(angle);
	return 1;
}

//function that checks if we may work on the rotation or not
int do_not_rotate(int ang, int poz, char *type,
				  int x1, int y1, int x2, int y2, int fs)
{
	if (ang % 360 == 0 || ang == 0) {
		print_rotate(ang, poz);
		return 0;
	}
	if (ang != 90 && ang != 180 && ang != 270) {
		printf("Unsupported rotation angle\n");
		return 0;
	}
	if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0) {
		if ((((x2 - 2) / 3) - (x1 / 3) + 1) != (y2 - y1 + 1) && fs == 0) {
			printf("The selection must be square\n");
			return 0;
		}
	} else {
		if ((x2 - x1 + 1) != (y2 - y1 + 1) && fs == 0) {
			printf("The selection must be square\n");
			return 0;
		}
	}
	return 1;
}

//function that tells if the image has been fully selected
int full_selection(int x, int y, int x1, int y1, int x2, int y2, char *type)
{
	//going one pixel back from the right and lower selection
	x2--;
	y2--;
	//checking if the image is fully selected
	if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0) {
		x2 -= 2;
		if (x1 == 0 && y1 == 0 && x2 + 1 == x && y2 + 1 == y)
			return 1;
	}
	if (strcmp(type, "P3") != 0 && strcmp(type, "P6") != 0)
		if (x1 == 0 && y1 == 0 && x2 + 1 == x && y2 + 1 == y)
			return 1;
	return 0;
}

//function that rotates noncolor images on the interior
int **interior_rotate(int **mat, int x, int y, int x1, int y1,
					  int x2, int y2, int ang, int poz, char *type)
{
	int i;
	int **matwork = (int **)malloc(y * sizeof(int *));
		for (i = 0; i < y; i++)
			matwork[i] = (int *)malloc(x * sizeof(int));
	if (strcmp(type, "P3") != 0 && strcmp(type, "P6") != 0) {
		//noncolor images
		if ((ang == 90 && poz == 1) || (ang == 270 && poz == 0)) {
			mat = right_rot_noncol(x1, y1, x2, y2, mat, matwork);
		} else {
			if (ang == 180)
				for (int k = 1; k <= 2; k++)
					mat = right_rot_noncol(x1, y1, x2, y2, mat, matwork);
			else
				for (int k = 1; k <= 3; k++)
					mat = right_rot_noncol(x1, y1, x2, y2, mat, matwork);
		}
	} else {
		//color images
		if ((ang == 90 && poz == 1) || (ang == 270 && poz == 0)) {
			mat = right_rot_col(x1, y1, x2, y2, mat, matwork);
		} else {
			if (ang == 180)
				for (int k = 1; k <= 2; k++)
					mat = right_rot_col(x1, y1, x2, y2, mat, matwork);
			else
				for (int k = 1; k <= 3; k++)
					mat = right_rot_col(x1, y1, x2, y2, mat, matwork);
		}
	}
	free_matrix(y, matwork);
	print_rotate(ang, poz);
	return mat;
}

//ROTATE function
int **rotate_fct(char *command, int *x, int *y, int x1, int y1,
				 int x2, int y2, int **mat, char type[3], int *fs)
{
	char angle[10] = "\0";
	int ang, poz, i;
	poz = poz_or_neg(angle, command, &ang);
	*fs = full_selection(*x, *y, x1, y1, x2, y2, type);
	x2--, y2--;
	if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
		x2 -= 2;
	if (do_not_rotate(ang, poz, type, x1, y1, x2, y2, *fs) == 0)
		return mat;
	if (*fs == 0) {
		//rotating inside the images
		mat = interior_rotate(mat, *x, *y, x1, y1, x2, y2, ang, poz, type);
		return mat;
	}
	if (*fs == 1) {
		//rotating full images
		if (strcmp(type, "P3") != 0 && strcmp(type, "P6") != 0) {
			//noncolor images
			int **matwork = (int **)malloc(*x * sizeof(int *));
			for (i = 0; i < *x; i++)
				matwork[i] = (int *)malloc(*y * sizeof(int));
			if (ang == 180) {
				matwork = full_right_rot_noncol(*x, *y, mat, matwork);
				mat = full_right_rot_noncol(*y, *x, matwork, mat);
				print_rotate(ang, poz);
				free_matrix(*x, matwork);
				return mat;
			}
			if ((ang == 90 && poz == 1) || (ang == 270 && poz == 0))
				matwork = full_right_rot_noncol(*x, *y, mat, matwork);
			if ((ang == 90 && poz == 0) || (ang == 270 && poz == 1))
				matwork = full_left_rot_noncol(*x, *y, mat, matwork);
			free_matrix(*y, mat);
			int aux = *x;
			*x = *y, *y = aux;
			print_rotate(ang, poz);
			return matwork;
		}
		if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0) {
			//color images
			int **matwork = (int **)malloc(*x / 3 * sizeof(int *));
			for (i = 0; i < *x / 3; i++)
				matwork[i] = (int *)malloc(*y * 3 * sizeof(int));
			if (ang == 180) {
				matwork = full_right_rot_col(*x, *y, mat, matwork);
				mat = full_right_rot_col(*y * 3, *x / 3, matwork, mat);
				print_rotate(ang, poz);
				free_matrix(*x / 3, matwork);
				return mat;
			}
			if ((ang == 90 && poz == 1) || (ang == 270 && poz == 0))
				matwork = full_right_rot_col(*x, *y, mat, matwork);
			if ((ang == 90 && poz == 0) || (ang == 270 && poz == 1))
				matwork = full_left_rot_col(*x, *y, mat, matwork);
			free_matrix(*y, mat);
			int aux = *x;
			*x = *y * 3, *y = aux / 3;
			print_rotate(ang, poz);
			return matwork;
		}
	}
	return mat;
}

void load_call(int ***mat, int *mxpix, int *first, int *cor, int *x1, int *y1,
			   int *x2, int *y2, int *x, int *y, char *maxpix,
			   char *command, char *file, char *type)
{
	*cor = 1;
	if (*first == 1)
		free_matrix(*y, *mat);
	if (check_file(command, file) == 1) {
		*mat = load_fct(command, type, &(*x), &(*y), maxpix);
		*x1 = 0, *y1 = 0, *x2 = *x, *y2 = *y, *first = 1;
		if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
			*x2 += 2;
		*mxpix = char_to_int(maxpix);
	} else {
		printf("Failed to load %s\n", file);
			*first = 0;
	}
}

void crop_call(int ***mat, int *first, int *cor, int *x1, int *y1,
			   int *x2, int *y2, int *x, int *y, char *type)
{
	*cor = 1;
	if (*first == 0) {
		printf("No image loaded\n");
	} else {
		printf("Image cropped\n");
		*mat = crop_fct(&(*x), &(*y), *x1, *y1, *x2, *y2, *mat, type);
		*x1 = 0, *y1 = 0, *x2 = *x, *y2 = *y;
		if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
			*x2 += 2;
	}
}

void select_call(int *first, int *cor, int *x1, int *y1,
				 int *x2, int *y2, int *x, int *y, char *command, char *type)
{
	*cor = 1;
	if (*first == 0) {
		printf("No image loaded\n");
	} else {
		if (check_select(command) == 1)
			select_fct(command, *x, *y, &(*x1), &(*y1), &(*x2), &(*y2), type);
		else
			printf("Invalid command\n");
	}
}

int main(void)
{
	char command[100], *com1, command_cp[100], type[3], maxpix[5], file[100];
	int **mat, x, y, first = 0, mxpix, cor = 0;
	int fs = 0, x1 = 0, x2 = 0, y1 = 0, y2 = 0;
	//reading first command
	fgets(command, 100, stdin);
	if (strcmp(command, "\n") != 0)
		command[strlen(command) - 1] = '\0';
	//resolving the commands here
	while (strncmp(command, "EXIT", 4) != 0) {
		cor = 0; //checking the command's first argument down
		strcpy(command_cp, command);
		com1 = strtok(command_cp, " ");
		strcat(com1, "\0");
		if (strcmp(com1, "LOAD") == 0)
			load_call(&mat, &mxpix, &first, &cor, &x1, &y1,
					  &x2, &y2, &x, &y, maxpix, command, file, type);
		if (strcmp(com1, "SELECT") == 0)
			select_call(&first, &cor, &x1, &y1, &x2, &y2,
						&x, &y, command, type);
		if (strcmp(com1, "CROP") == 0)
			crop_call(&mat, &first, &cor, &x1, &y1, &x2, &y2, &x, &y, type);
		if (strcmp(com1, "SAVE") == 0) {
			cor = 1;
			if (first == 0)
				printf("No image loaded\n");
			else
				save_fct(command, mat, x, y, maxpix, type);
		}
		if (strcmp(com1, "ROTATE") == 0) {
			cor = 1;
			if (first == 0) {
				printf("No image loaded\n");
			} else {
				mat = rotate_fct(command, &x, &y, x1, y1,
								 x2, y2, mat, type, &fs);
				if (fs == 1) {
					x1 = 0, y1 = 0, x2 = x, y2 = y;
					if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0)
						x2 += 2;
				}
			}
		}
		if (strcmp(com1, "GRAYSCALE") == 0) {
			cor = 1;
			if (first == 0) {
				printf("No image loaded\n");
			} else {
				if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0) {
					printf("Grayscale filter applied\n");
					mat = grayscale_fct(x1, y1, x2, y2, mat);
				} else {
					printf("Grayscale filter not available\n");
				}
			}
		}
		if (strcmp(com1, "SEPIA") == 0) {
			cor = 1;
			if (first == 0) {
				printf("No image loaded\n");
			} else {
				if (strcmp(type, "P3") == 0 || strcmp(type, "P6") == 0) {
					printf("Sepia filter applied\n");
					mat = sepia_fct(x1, y1, x2, y2, mat, mxpix);
				} else {
					printf("Sepia filter not available\n");
				}
			}
		}
		if (cor == 0)
			printf("Invalid command\n"); //reading next command down
		fgets(command, 100, stdin);
		if (strcmp(command, "\n") != 0 && strncmp(command, "EXIT", 4) != 0)
			command[strlen(command) - 1] = '\0';
	}
	if (first == 1)
		free_matrix(y, mat);
	else
		printf("No image loaded\n");
	return 0;
}
