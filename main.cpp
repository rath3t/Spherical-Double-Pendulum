#include <Eigen/Core>
#include <Eigen/Dense>



#include <iostream>
using Matrix32d = Eigen::Matrix<double,3,2>;

const Eigen::Vector3d gravitation = (Eigen::Vector3d() <<0 , 0, -9.81 ).finished();

struct GeometryAndMaterial
{
  Eigen::Vector2d pointMasses;
  Eigen::Vector2d trussLength;
};

double kineticEnergy(const Eigen::Vector<double,6>& v,const GeometryAndMaterial& geoMat)
{

  return 0.5 * (geoMat.pointMasses(0)*geoMat.trussLength(0)*geoMat.trussLength(0)*v.segment<3>(0).squaredNorm() +
      geoMat.pointMasses(1)*(geoMat.trussLength(0)*v.segment<3>(0)+geoMat.trussLength(1)*v.segment<3>(3)).squaredNorm());
}

double potentialEnergy(const Eigen::Vector<double,6>& t,const GeometryAndMaterial& geoMat)
{
  return geoMat.pointMasses(0)*geoMat.trussLength(0)* gravitation.dot(t.segment<3>(0))+ geoMat.pointMasses(0)*gravitation.dot(geoMat.trussLength(0)*t.segment<3>(0)+geoMat.trussLength(1)*t.segment<3>(1));
}

Matrix32d calculateTangentSpace(const Eigen::Vector3d& t)
{
  Matrix32d BLA;
  double st = (t(2) > 0) ? 1.0 : ((t(2) < 0) ? -1.0 : 0.0);

  const double  s  = 1/(1+st*(t[2]));
  const Eigen::Vector2d y = (Eigen::Vector2d() <<t[0] * s, t[1] * s ).finished();

  const double ys1 = y[0]*y[0];
  const double ys2 = y[1]*y[1];
  const double s2 = 2.0*(1.0+ys1+ys2);

  BLA <<     s2-4.0*ys1, -4.0*y[0]*y[1],
      -4.0*y[0]*y[1],     s2-4.0*ys2,
      -st*4.0*y[0],   -st*4.0*y[1];

  BLA.col(0)=BLA.col(0).normalized();
  BLA.col(1)=BLA.col(1).normalized();
  return BLA;
}

Eigen::Vector3d retraction(const Eigen::Vector3d& t,const Eigen::Vector3d& dt)
{
  const double normdt = dt.norm();
  return cos(normdt)*t+sin(normdt)/normdt*dt;
}

Eigen::Matrix<double,6,6> rotationMatrixfromtwoVectorsLarge(const Eigen::Vector<double,6>& tn, const Eigen::Vector<double,6>& t)
{
  Eigen::Matrix<double,6,6> R;
  R.block<3,3>(0,0) = Eigen::Quaterniond().setFromTwoVectors(tn.segment<3>(0),t.segment<3>(0)).matrix();
  R.block<3,3>(3,3) = Eigen::Quaterniond().setFromTwoVectors(tn.segment<3>(3),t.segment<3>(3)).matrix();
  return R;
}


Eigen::Matrix<double,6,6> massMatrix(const GeometryAndMaterial& geoMat)
{
  Eigen::Matrix<double,6,6> M;
  M.topLeftCorner<3,3>() = (geoMat.pointMasses(0)+geoMat.pointMasses(1))*geoMat.trussLength(0)*geoMat.trussLength(0)*Eigen::Matrix3d::Identity();
  M.bottomLeftCorner<3,3>() = geoMat.pointMasses(1)*geoMat.trussLength(0)*geoMat.trussLength(1)*Eigen::Matrix3d::Identity();
  M.topRightCorner<3,3>() = M.bottomLeftCorner<3,3>();
  M.bottomRightCorner<3,3>() = geoMat.pointMasses(1)*geoMat.trussLength(1)*geoMat.trussLength(1)*Eigen::Matrix3d::Identity();
  return M;
}

Eigen::Vector<double,6> velocityTerms(const Eigen::Vector<double,6>& t, const Eigen::Vector<double,6>& v,const GeometryAndMaterial& geoMat)
{

Eigen::Vector<double,6> c;

c.segment<3>(0)=-t.segment<3>(3)*v.segment<3>(3).squaredNorm();
c.segment<3>(3)=-t.segment<3>(0)*v.segment<3>(0).squaredNorm();



c*=geoMat.pointMasses(1)*geoMat.trussLength(0)*geoMat.trussLength(0);

return c;
}

Eigen::Matrix<double,6,4> createLargeTangentBase(const Eigen::Vector<double,6>& t)
{
  Eigen::Matrix<double,6,4> BLA;
  BLA.block<3,2>(0,0) = calculateTangentSpace(t.segment<3>(0));
  BLA.block<3,2>(3,2) = calculateTangentSpace(t.segment<3>(3));
  return BLA;
}

Eigen::Vector<double,6> gradientPotentialEnergy(const GeometryAndMaterial& geoMat)
{
  return (Eigen::Vector<double,6>() << (geoMat.pointMasses(0)+geoMat.pointMasses(1))*geoMat.trussLength(0)*gravitation,geoMat.pointMasses(1)*geoMat.trussLength(2)*gravitation).finished();
}

int main() {

double time =0.0;
int  timesteps =100;
double deltat =  0.005;

double   endtime =  timesteps*deltat;
double gamma = 1.5; //Chung Lee time integration
  double beta = 1.5;
  double a0 = 0.0; //how much damping

  GeometryAndMaterial geoMat;

  geoMat.trussLength <<  1,1;
  geoMat.pointMasses <<  1,1;
  //Init




  Eigen::Vector<double,6> t;
  t << 1.0,1.0,1.0,     1.0,2.0,8.0;

t.col(0).normalize();
t.col(1).normalize();

  Eigen::Vector<double,6> v = Eigen::Vector<double,6>::Zero();
  Eigen::Vector<double,6> a  = Eigen::Vector<double,6>::Zero();


const Eigen::Matrix<double,6,6> M = massMatrix(geoMat);
const Eigen::Matrix<double,6,6> C = a0*M; //Mass propertional damping matrix

//time integration

while(time<endtime)
{
  const Eigen::Matrix<double,6,4> BLA = createLargeTangentBase(t);
  const Eigen::Matrix4d Mred = BLA.transpose()*M*BLA;
  const Eigen::Vector<double,6> velocityTerm = velocityTerms(t,v,geoMat);
  const Eigen::Vector<double,6> rhs = -C*v-velocityTerm+gradientPotentialEnergy(geoMat);
  const Eigen::Vector<double,4> rhsRed = BLA.transpose()*rhs;

  const Eigen::Vector<double,4> aRedn = BLA.transpose()*a;
  const Eigen::Vector<double,4> aRed = Mred.inverse()*rhsRed ;

  const Eigen::Vector<double,4> vRedn = BLA.transpose()*v;

  const Eigen::Vector<double,4> vRed = vRedn+ deltat*((1-gamma)*aRedn+gamma*aRed) ;


  const Eigen::Vector<double,4> deltaVec2d = deltat* vRedn + deltat*deltat*((0.5-beta)*aRedn+beta*aRed);
  const Eigen::Vector<double,6> deltaVec3d = BLA*deltaVec2d;

  const Eigen::Vector<double,6> tn = t;
  t.segment<3>(0) = retraction(t.segment<3>(0),deltaVec3d.segment<3>(0));
  t.segment<3>(3) = retraction(t.segment<3>(3),deltaVec3d.segment<3>(3));


  const Eigen::Matrix<double,6,6> PTR = rotationMatrixfromtwoVectorsLarge(tn,t);

  v = PTR *(BLA*vRed);
  a = PTR *(BLA*aRed);

  std::cout<<potentialEnergy(t,geoMat)<< "   "<<kineticEnergy(v,geoMat)<<"    "<<time<< "      "<<potentialEnergy(t,geoMat)+kineticEnergy(v,geoMat)<< '\n';
  time+=deltat;
}


return 0;
}
