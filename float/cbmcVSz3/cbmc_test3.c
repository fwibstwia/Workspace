int main(){
	float rx, ry, rz, sx, sy, sz, radius;
	float A = rx*rx+ry*ry+rz*rz;
	float B = -2.0*(sx*rx+sy*ry+sz*rz);
	float C = sx*sx+sy*sy+sz*sz - 14;
	float D = B*B - 4.0*A*C;
	if(sx > 1 && sx < 10 && sy > 1 && sy < 10 
	   && sz > 1 && sz < 10 && rx > 1 && rx < 10 
           && ry > 1 && ry < 10 && rz > 1 && rz < 10){
		assert(D != 0);
	}	
}
