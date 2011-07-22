#include "common.h"

float particle_R []={.0035, .012,.0005, .0017};
float particle_Rho []={1560, 720, 5490, 5490};
float particle_Theta []={23,  28,   24,   24};
float impactor_R []={.095, .013, .015, .020, .025, .035, .040, .045, .050, .013, .019, .026, .0125, .019};
float impactor_M []={.034, .083, .130, .287, .531,1.437,2.099,3.055,4.079, .064, .201, .518,  .009, .018};

float container_R=   .3;
float container_T=  .03;


ChSharedPtr<ChLinkLockPointLine> my_motor;
ChSharedBodyGPUPtr Anchor;
void System::DoTimeStep(){


//	if(mNumCurrentObjects<mNumObjects&&mFrameNumber%50==0){
//		float x=35;	float posX=0;
//		float y=10;	float posY=-20;
//		float z=35;	float posZ=0;
//
//		float radius	=.5;
//		float mass	=1;
//		float mu	=.5;
//		float rest	=0;
//		int type 	=0;
//		ChSharedBodyGPUPtr mrigidBody;
//		mNumCurrentObjects+=x*y*z;
//		int mobjNum=0;
//
//		for (int xx=0; xx<x; xx++){
//		for (int yy=0; yy<y; yy++){
//		for (int zz=0; zz<z; zz++){
//			ChVector<> mParticlePos((xx-(x-1)/2.0)+posX,(yy)+posY,(zz-(z-1)/2.0)+posZ);
//			mParticlePos+=ChVector<>(rand()%1000/1000.0-.5,rand()%1000/1000.0-.5,rand()%1000/1000.0-.5);
//			ChQuaternion<> quat=ChQuaternion<>(1,0,0,0);;
//			mrigidBody = ChSharedBodyGPUPtr(new ChBodyGPU);
//			if(type==0){MakeSphere(mrigidBody, radius, mass, mParticlePos*1.2, mu, mu, rest, true);}
//			if(type==1){MakeBox(mrigidBody, ChVector<>(radius,radius,radius), mass, mParticlePos,quat, mu, mu, rest,mobjNum,mobjNum,true, false);}
//			if(type==2){MakeEllipsoid(mrigidBody, ChVector<>(radius,radius,radius), mass, mParticlePos,quat, mu, mu, rest, true);}
//			mobjNum++;
//			}
//		}
//	}
//
//	}

//if(mCurrentTime<4){
//	Anchor->Set_Scr_torque(ChVector<>(0,-10,0));
//	Anchor->Set_Scr_force(ChVector<>(0,-10,0));
//}
//if(mCurrentTime>5&&mCurrentTime<7){
//	Anchor->Set_Scr_torque(ChVector<>(0,0,0));
//	Anchor->Set_Scr_force(ChVector<>(0,20,0));
//}

	DeactivationPlane(-40);
	
	if(mFrameNumber%3==0&&saveData){
		SaveAllData("data/anchor",true, false, false, true, false);
	}
	
	
	//if(mSystem->GetChTime()>5){
	//	BTM->SetBodyFixed(false);
	//	BTM->SetCollide(true);
	//}
	mFrameNumber++;
	mSystem->DoStepDynamics( mTimeStep );
	mCurrentTime+=mTimeStep;
}

int main(int argc, char* argv[]){
	Scale=.01;
	float mOmega=.1;
	int   mIteations=200;
	float mTimeStep=.0001;
	float mEnvelope=0;
	float mMu=.5;
	float mWallMu=.5;
	int   mDevice=0;
	float mEndTime=20;
	bool OGL=0;
	if(argc==3){OGL=atoi(argv[1]);	saveData=atoi(argv[2]);}

	cudaSetDevice(mDevice);
	
	ChLcpSystemDescriptorGPU		mGPUDescriptor;
	ChContactContainerGPUsimple		mGPUContactContainer;
	ChCollisionSystemGPU			mGPUCollisionEngine(&mGPUDescriptor, mEnvelope);
	ChLcpIterativeSolverGPUsimple	mGPUsolverSpeed(&mGPUContactContainer,&mGPUDescriptor,  mIteations,mTimeStep, 1e-5, mOmega, false);
	
	ChSystemGPU SysG(1000, 50);
	SysG.ChangeLcpSystemDescriptor(&mGPUDescriptor);
	SysG.ChangeContactContainer(&mGPUContactContainer);
	SysG.ChangeLcpSolverSpeed(&mGPUsolverSpeed);
	SysG.ChangeCollisionSystem(&mGPUCollisionEngine);
	SysG.SetIntegrationType(ChSystem::INT_ANITESCU);
	SysG.Set_G_acc(ChVector<>(0,GRAV,0));
	GPUSystem=new System(&SysG,10,"anchor.txt");
	GPUSystem->mMu=mMu;
	GPUSystem->mTimeStep=mTimeStep;
	GPUSystem->mEndTime=mEndTime;
	ChQuaternion<> base(1,0,0,0);
	Anchor			=	ChSharedBodyGPUPtr(new ChBodyGPU);
	ChSharedBodyGPUPtr L	=	ChSharedBodyGPUPtr(new ChBodyGPU);
	ChSharedBodyGPUPtr R	=	ChSharedBodyGPUPtr(new ChBodyGPU);
	ChSharedBodyGPUPtr F	=	ChSharedBodyGPUPtr(new ChBodyGPU);
	ChSharedBodyGPUPtr B	=	ChSharedBodyGPUPtr(new ChBodyGPU);
	ChSharedBodyGPUPtr BTM	=	ChSharedBodyGPUPtr(new ChBodyGPU);

	GPUSystem->MakeBox(L,	ChVector<>(container_T,container_R,container_R), 100000,ChVector<>(-container_R,0,0),base,mWallMu,mWallMu,0,-20,-20,true,true);
	GPUSystem->MakeBox(R,	ChVector<>(container_T,container_R,container_R), 100000,ChVector<>(container_R,0,0), base,mWallMu,mWallMu,0,-20,-20,true,true);
	GPUSystem->MakeBox(F,	ChVector<>(container_R,container_R,container_T), 100000,ChVector<>(0,0,-container_R),base,mWallMu,mWallMu,0,-20,-20,true,true);
	GPUSystem->MakeBox(B,	ChVector<>(container_R,container_R,container_T), 100000,ChVector<>(0,0,container_R), base,mWallMu,mWallMu,0,-20,-20,true,true);
	GPUSystem->MakeBox(BTM, ChVector<>(container_R,container_T,container_R), 100000,ChVector<>(0,-container_R,0),base,mWallMu,mWallMu,0,-20,-20,true,true);
	
	ChQuaternion<> anchor_rot;
	float angle =-90*PI/180.0;
	ChVector<double> axis(1,0.,0);
	anchor_rot.Q_from_AngAxis(angle,axis);

	//GPUSystem->LoadTriangleMesh(Anchor,"Helical_Anchor2.obj",.5,ChVector<> (0,0,0),anchor_rot,1,.5,.5,0,-21,-21 );
	GPUSystem->MakeEllipsoid(Anchor, ChVector<>(.05,.1,.05), .5, ChVector<> (0,0,0),base, .5, .5, 0, true);
	
	float radius	=particle_R[0];
			float mass	=particle_Rho[0]*4.0/3.0*PI*radius*radius*radius;
			float mu	=tan(particle_Theta[0]*PI/180.0);
			float rest	=0;
			int   type 	=0;


	ifstream ifile("ball_drop_start.txt");
	string data;
	for(int i=0; i<0; i++){
	getline(ifile,data);
	if(i>=5){
	for(int j=0; j<data.size(); j++){
		if(data[j]==','){data[j]='\t';}
	}
	stringstream ss(data);
	float x,y,z,rx,ry,rz;
	ss>>x>>y>>z>>rx>>ry>>rz;
	ChSharedBodyGPUPtr mrigidBody;
	mrigidBody = ChSharedBodyGPUPtr(new ChBodyGPU);
	GPUSystem->MakeSphere(mrigidBody, radius, 1560*4.0/3.0*PI*radius*radius*radius, ChVector<>(x,y,z), mu, mu, rest, true);
	}
	}

	//GPUSystem->MakeSphere(I, 5, .03, ChVector<>(0,0,0), mMu, mMu, 0, true);
	
	//my_motor= ChSharedPtr<ChLinkLockPointLine> (new ChLinkLockPointLine);
	//ChSharedBodyPtr ptr1=ChSharedBodyPtr(Anchor);
	//ChSharedBodyPtr ptr2=ChSharedBodyPtr(BTM);
	
	//my_motor->Initialize(ptr1,ptr2,ChCoordsys<>(ChVector<>(0,1,0)));
	//ChFunction_Sine *vibrationFunc=new ChFunction_Sine(0,50,1);
	//my_motor->SetMotion_Y(vibrationFunc);
	//SysG.AddLink(my_motor);
	//my_motor->SetDisabled(true);
	//BTM->SetBodyFixed(true);
	//BTM->SetCollide(false);
#pragma omp parallel sections
	{
#pragma omp section
		{
			while(GPUSystem->mSystem->GetChTime()<=GPUSystem->mEndTime){GPUSystem->renderScene();}
		}
#pragma omp section
		{
			if(OGL){initGLUT(string("anchor"),argc, argv);}
		}
	}
	return 0;
}
