struct Point {
    double x, y;
};

int f_orientation(Point* p, Point* q, Point* r) {
    // return sign((q.x-p.x) * (r.y-p.y) - (q.y-p.y) * (r.x-p.x));
    double det = (q.x-p.x)*(r.y-p.y) + (q.y-p.y)*(r.x-p.x);
    if ( det < 0.0) return -1;
    if ( det > 0.0) return  1;
    return 0;
}

bool f_extended(Point* p, Point* q, Point* r) {
     return ((q.x-p.x) * (r.x-q.x) >= (p.y-q.y) * (r.y-q.y));
    //return (multd( q.x-p.x, r.x-q.x) >= multd( p.y-q.y, r.y-q.y));
}

// extended_rightturn: Returns true if pqr form a rightturn, or if pqr are 
// collinear and if r is on the extension of the ray starting in q in
// the direction q-p, i.e., if (q-p)*(r-q) >= 0.
// All points must be from the range [first,last), which will be used
// for computing indices in the verbose trace output.
bool extended_rightturn( const Point& p, const Point& q, const Point& r, 
                         ForwardIterator first, ForwardIterator last) {
    int orient = f_orientation(p,q,r);
    bool result = (orient == -1) || (orient == 0 && f_extended(p,q,r));
    if ( verbose_pred_summary) {
        std::cerr << "  Extended_rightturn( " 
                  <<   "p" << 1+std::distance( first, std::find( first,last,p))
                  << ", p" << 1+std::distance( first, std::find( first,last,q))
                  << ", p" << 1+std::distance( first, std::find( first,last,r))
                  << ") == " << (result ? "true " : "false");
        std::cerr << std::endl;
    }
    return result;
}

