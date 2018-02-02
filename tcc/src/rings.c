#include "rings.h"
#include "globals.h"
#include "bonds.h"

void Rings_gSP4(int f, int n0, int n1, int n2) {    // {n0,n1,n2} is not an SP3 ring, is it an SP4 or SP5 ring?
    int i;
    int n3;
    
    for (i=0; i<cnb[n1]; ++i) {
        n3=bNums[n1][i];
        if (n3 <= n0) continue;
        if (!Bonds_BondCheck(n0,n3)) {  // n1 not bonded to n2 & n0 not bonded to n3
            if (Bonds_BondCheck(n2,n3)) { // 4 membered ring found 
                Rings_aSP4(f,n0,n1,n3,n2); // check SP4 type and store  
            }
            else{ // n1 not bonded to n3
                if (dosp5==1) Rings_gSP5(f,n0,n1,n3,n2);
            }
        }       
    }
}

void Rings_gSP5(int f, int n0, int n1, int n2, int n3) {    // {n0,n1,n2,n3} is not an SP4 ring, is it an SP5 ring?
    int i,j;
    int n4,n5;
    int bond4_1;
    
    for (i=0; i<cnb[n2]; ++i){
        n4=bNums[n2][i];
        if(n4 < n0 || n4 == n3) continue; // Now: is n4 bonded to n1 and not to n2 or n0
        bond4_1 = 0;
        for (j=0; j<cnb[n4]; ++j){
            n5=bNums[n4][j];
            if (n5==n3) bond4_1 = 1;
            if (n5==n1 || n5==n0) break; // Not SP ring
        }
        if (j==cnb[n4] && bond4_1==1) {
            Rings_aSP5(f,n0, n1, n2, n4, n3); // check SP5 type and store 
        }
    }
}

void Rings_aSP3(int f, int n0, int n1, int n2) {    // Take {n0,n1,n2}, check SP3 ring and if so detect SP3a/b/c cluster
    int i, j;
    int type = 0;
    int cp[2];  // common spindles - particles bonded to all members of three membered ring
    int bcheck;
    int binAcnt;

    cp[0]=cp[1]=-1;
    for (i=0; i<cnb[n0]; ++i) {
        j = bNums[n0][i];
        bcheck = j == n1 || j == n2;
        if (bcheck) continue;
        bcheck = Bonds_BondCheck(n1,j)==1 && Bonds_BondCheck(n2,j)==1;
        if (bcheck) {
            if (type<2) {
                cp[type] = j;
                type++;
            }
            else type++;
        }
    }
    
    if (maxto3<type) maxto3=type;
    
    if (type==0 && dosp3a==1) {
        if (nsp3a[f] == msp3a) {
            sp3a=resize_2D_int(sp3a,msp3a,msp3a+incrStatic,3,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp3a=resize_1D_double(bl_mom_sp3a,msp3a,msp3a+incrStatic);
                bl_mom_sp3=resize_1D_double(bl_mom_sp3,msp3,msp3+incrStatic);
                msp3=msp3+incrStatic;
            }
            msp3a=msp3a+incrStatic;
        }
        sp3a[nsp3a[f]][0] = n0;
        sp3a[nsp3a[f]][1] = n1;
        sp3a[nsp3a[f]][2] = n2;
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3a,&BLDistroNoSamplessp3a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3a,&BLDistroNoSamplessp3a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3a,&BLDistroNoSamplessp3a);
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
        }
        
        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n2]==1) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n2]==2) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB);
            }
            
            bl_mom_sp3a[nsp3a[f]]/=3.0;
            bl_mom_sp3[nsp3[f]]/=3.0;
        }
        
        ++nsp3a[f];
    }
    else if (type==1 && dosp3b==1) {
        if (nsp3b[f] == msp3b) { 
            sp3b=resize_2D_int(sp3b,msp3b,msp3b+incrStatic,4,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp3b=resize_1D_double(bl_mom_sp3b,msp3b,msp3b+incrStatic);
                bl_mom_sp3=resize_1D_double(bl_mom_sp3,msp3,msp3+incrStatic);
                msp3=msp3+incrStatic;
            }
            msp3b=msp3b+incrStatic;
        }
        sp3b[nsp3b[f]][0] = n0;
        sp3b[nsp3b[f]][1] = n1;
        sp3b[nsp3b[f]][2] = n2;
        sp3b[nsp3b[f]][3] = cp[0];

        mem_sp3b[n0][nmem_sp3b[n0]]=nsp3b[f];       
        nmem_sp3b[n0]++;        
        if (nmem_sp3b[n0] >= mmem_sp3b) {
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3b[binAcnt]=resize_1D_int(mem_sp3b[binAcnt],mmem_sp3b,mmem_sp3b+incrClustPerPart);
            }
            mmem_sp3b=mmem_sp3b+incrClustPerPart;
        }
        
        mem_sp3b[n1][nmem_sp3b[n1]]=nsp3b[f];       
        nmem_sp3b[n1]++;        
        if (nmem_sp3b[n1] >= mmem_sp3b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3b[binAcnt]=resize_1D_int(mem_sp3b[binAcnt],mmem_sp3b,mmem_sp3b+incrClustPerPart);
            }
            mmem_sp3b=mmem_sp3b+incrClustPerPart;
        }
        
        mem_sp3b[n2][nmem_sp3b[n2]]=nsp3b[f];       
        nmem_sp3b[n2]++;        
        if (nmem_sp3b[n2] >= mmem_sp3b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3b[binAcnt]=resize_1D_int(mem_sp3b[binAcnt],mmem_sp3b,mmem_sp3b+incrClustPerPart);
            }
            mmem_sp3b=mmem_sp3b+incrClustPerPart;
        }
        
        mem_sp3b[cp[0]][nmem_sp3b[cp[0]]]=nsp3b[f]; 
        nmem_sp3b[cp[0]]++;     
        if (nmem_sp3b[cp[0]] >= mmem_sp3b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3b[binAcnt]=resize_1D_int(mem_sp3b[binAcnt],mmem_sp3b,mmem_sp3b+incrClustPerPart);
            }
            mmem_sp3b=mmem_sp3b+incrClustPerPart;
        }
        
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3b,&BLDistroNoSamplessp3b);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3b,&BLDistroNoSamplessp3b);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3b,&BLDistroNoSamplessp3b);
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)],BLDistrosp3b,&BLDistroNoSamplessp3b);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)],BLDistrosp3b,&BLDistroNoSamplessp3b);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)],BLDistrosp3b,&BLDistroNoSamplessp3b);
        }
        
        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3b)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3b);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3b*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3b*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3b*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3b*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n2]==1) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3b)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3b);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n2]==2) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3b*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3b*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3b*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3b*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3b)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3b);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3b*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3b*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3b*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3b*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[cp[0]]==1 && rtype[n0]==1) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3b)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3b);
            }
            else if (rtype[cp[0]]==2 && rtype[n0]==2) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3b*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3b*sigma_BB);
            }
            else {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3b*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3b*sigma_AB);
            }
            
            if (rtype[cp[0]]==1 && rtype[n1]==1) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3b)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3b);
            }
            else if (rtype[cp[0]]==2 && rtype[n1]==2) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3b*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3b*sigma_BB);
            }
            else {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3b*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3b*sigma_AB);
            }
            
            if (rtype[cp[0]]==1 && rtype[n2]==1) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3b)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3b);
            }
            else if (rtype[cp[0]]==2 && rtype[n2]==2) {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3b*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3b*sigma_BB);
            }
            else {
                bl_mom_sp3b[nsp3b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3b*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3b*sigma_AB);
            }
            
            bl_mom_sp3b[nsp3b[f]]/=6.0;
            bl_mom_sp3[nsp3[f]]/=3.0;
        }
        
        ++nsp3b[f];
    }
    else if (type==2 && dosp3c==1) {
        if (nsp3c[f] == msp3c) { 
            sp3c=resize_2D_int(sp3c,msp3c,msp3c+incrStatic,5,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp3c=resize_1D_double(bl_mom_sp3c,msp3c,msp3c+incrStatic);
                bl_mom_sp3=resize_1D_double(bl_mom_sp3,msp3,msp3+incrStatic);
                msp3=msp3+incrStatic;
            }
            msp3c=msp3c+incrStatic;
        }
        sp3c[nsp3c[f]][0] = n0;
        sp3c[nsp3c[f]][1] = n1;
        sp3c[nsp3c[f]][2] = n2; 
        if (cp[0]<cp[1]) {
            sp3c[nsp3c[f]][3] = cp[0];
            sp3c[nsp3c[f]][4] = cp[1];
        }
        else {
            sp3c[nsp3c[f]][3] = cp[1];
            sp3c[nsp3c[f]][4] = cp[0];
        }

        mem_sp3c[n0][nmem_sp3c[n0]]=nsp3c[f];       
        nmem_sp3c[n0]++;        
        if (nmem_sp3c[n0] >= mmem_sp3c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3c[binAcnt]=resize_1D_int(mem_sp3c[binAcnt],mmem_sp3c,mmem_sp3c+incrClustPerPart);
            }
            mmem_sp3c=mmem_sp3c+incrClustPerPart;
        }
        
        mem_sp3c[n1][nmem_sp3c[n1]]=nsp3c[f];       
        nmem_sp3c[n1]++;        
        if (nmem_sp3c[n1] >= mmem_sp3c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3c[binAcnt]=resize_1D_int(mem_sp3c[binAcnt],mmem_sp3c,mmem_sp3c+incrClustPerPart);
            }
            mmem_sp3c=mmem_sp3c+incrClustPerPart;
        }
        
        mem_sp3c[n2][nmem_sp3c[n2]]=nsp3c[f];       
        nmem_sp3c[n2]++;        
        if (nmem_sp3c[n2] >= mmem_sp3c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3c[binAcnt]=resize_1D_int(mem_sp3c[binAcnt],mmem_sp3c,mmem_sp3c+incrClustPerPart);
            }
            mmem_sp3c=mmem_sp3c+incrClustPerPart;
        }
        
        mem_sp3c[cp[0]][nmem_sp3c[cp[0]]]=nsp3c[f]; 
        nmem_sp3c[cp[0]]++;     
        if (nmem_sp3c[cp[0]] >= mmem_sp3c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3c[binAcnt]=resize_1D_int(mem_sp3c[binAcnt],mmem_sp3c,mmem_sp3c+incrClustPerPart);
            }
            mmem_sp3c=mmem_sp3c+incrClustPerPart;
        }
        
        mem_sp3c[cp[1]][nmem_sp3c[cp[1]]]=nsp3c[f]; 
        nmem_sp3c[cp[1]]++;     
        if (nmem_sp3c[cp[1]] >= mmem_sp3c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp3c[binAcnt]=resize_1D_int(mem_sp3c[binAcnt],mmem_sp3c,mmem_sp3c+incrClustPerPart);
            }
            mmem_sp3c=mmem_sp3c+incrClustPerPart;
        }
        
        
        if (Bonds_BondCheck(sp3c[nsp3c[f]][3],sp3c[nsp3c[f]][4])==1) nsp3c_spindlebonds[f]++;
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            
            Bonds_TickBLDistro(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            Bonds_TickBLDistro(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            Bonds_TickBLDistro(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)],BLDistrosp3c,&BLDistroNoSamplessp3c);
            
            if (Bonds_BondCheck(cp[0],cp[1])==1) Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],cp[1])],BLDistrosp3c,&BLDistroNoSamplessp3c);
        }

        
        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3c)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3c);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3c*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3c*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3c*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3c*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n2]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3c)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3c);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n2]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3c*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3c*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3c*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3c*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3c)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3c);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3c*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3c*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3c*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3c*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[cp[0]]==1 && rtype[n0]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3c)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3c);
            }
            else if (rtype[cp[0]]==2 && rtype[n0]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3c*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3c*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3c*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_sp3c*sigma_AB);
            }
            
            if (rtype[cp[0]]==1 && rtype[n1]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3c)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3c);
            }
            else if (rtype[cp[0]]==2 && rtype[n1]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3c*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3c*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3c*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_sp3c*sigma_AB);
            }
            
            if (rtype[cp[0]]==1 && rtype[n2]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3c)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3c);
            }
            else if (rtype[cp[0]]==2 && rtype[n2]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3c*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3c*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3c*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_sp3c*sigma_AB);
            }
            
            if (rtype[cp[1]]==1 && rtype[n0]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_sp3c)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_sp3c);
            }
            else if (rtype[cp[1]]==2 && rtype[n0]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_sp3c*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_sp3c*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_sp3c*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_sp3c*sigma_AB);
            }
            
            if (rtype[cp[1]]==1 && rtype[n1]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_sp3c)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_sp3c);
            }
            else if (rtype[cp[1]]==2 && rtype[n1]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_sp3c*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_sp3c*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_sp3c*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_sp3c*sigma_AB);
            }
            
            if (rtype[cp[1]]==1 && rtype[n2]==1) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_sp3c)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_sp3c);
            }
            else if (rtype[cp[1]]==2 && rtype[n2]==2) {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_sp3c*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_sp3c*sigma_BB);
            }
            else {
                bl_mom_sp3c[nsp3c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_sp3c*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_sp3c*sigma_AB);
            }
            
            bl_mom_sp3c[nsp3c[f]]/=9.0;
            bl_mom_sp3[nsp3[f]]/=3.0;
        }
        
        ++nsp3c[f];
    }
    else if (dosp3a==1) {
        if (nsp3a[f] == msp3a) { 
            sp3a=resize_2D_int(sp3a,msp3a,msp3a+incrStatic,3,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp3a=resize_1D_double(bl_mom_sp3a,msp3a,msp3a+incrStatic);
                bl_mom_sp3=resize_1D_double(bl_mom_sp3,msp3,msp3+incrStatic);
                msp3=msp3+incrStatic;
            }
            msp3a=msp3a+incrStatic;
        }
        sp3a[nsp3a[f]][0] = n0;
        sp3a[nsp3a[f]][1] = n1;
        sp3a[nsp3a[f]][2] = n2;
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3a,&BLDistroNoSamplessp3a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3a,&BLDistroNoSamplessp3a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
            
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3a,&BLDistroNoSamplessp3a);
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,n2)],BLDistrosp3,&BLDistroNoSamplessp3);
        }
        
        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3a*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n2]==1) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3);
            }
            else if (rtype[n0]==2 && rtype[n2]==2) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3a*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n2)]-gsbl_sp3*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_BB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_BB);
            }
            else {
                bl_mom_sp3a[nsp3a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3a*sigma_AB);
                bl_mom_sp3[nsp3[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp3*sigma_AB);
            }
            
            bl_mom_sp3a[nsp3a[f]]/=3.0;
            bl_mom_sp3[nsp3[f]]/=3.0;
        }
        
        ++nsp3a[f];
        ++nsp3_excess_spindles[f];
    }
    
    ++nsp3[f];
}

void Rings_aSP4(int f, int n0, int n1, int n2, int n3) {    // Take {n0,n1,n2,n3}, check SP4 ring and if so detect SP4a/b/c cluster
    int i, j;
    int type = 0;
    int cp[2];  // common spindles - particles bonded to all members of three membered ring
    int bcheck;
    int flg;
    int trial[6];
    int binAcnt;

    cp[0]=cp[1]=-1;
    for (i=0; i<cnb[n0]; ++i) {
        j = bNums[n0][i];
        bcheck = j == n1 || j == n3;
        if (bcheck) continue;
        bcheck = Bonds_BondCheck(n1,j)==1 && Bonds_BondCheck(n2,j)==1 && Bonds_BondCheck(n3,j)==1;
        if (bcheck) {
            if (type<2) {
                cp[type] = j;
                type++;
            }
            else type++;
        }
    }
    
    if (maxto4<type) maxto4=type;
    
    if (type==0 && dosp4a==1) {
        if (nsp4a[f] == msp4a) { 
            sp4a=resize_2D_int(sp4a,msp4a,msp4a+incrStatic,4,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp4a=resize_1D_double(bl_mom_sp4a,msp4a,msp4a+incrStatic);
                bl_mom_sp4=resize_1D_double(bl_mom_sp4,msp4,msp4+incrStatic);
                msp4=msp4+incrStatic;
            }
            msp4a=msp4a+incrStatic;
        }
        sp4a[nsp4a[f]][0] = n0;
        sp4a[nsp4a[f]][1] = n1;
        sp4a[nsp4a[f]][2] = n2;
        sp4a[nsp4a[f]][3] = n3;
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
        }

        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n3]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n3]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB);
            }
            
            bl_mom_sp4a[nsp4a[f]]/=4.0;
            bl_mom_sp4[nsp4[f]]/=4.0;
        }
        
        ++nsp4a[f];
    }
    else if (type==1 && dosp4b==1) {
        if (nsp4b[f] == msp4b) { 
            sp4b=resize_2D_int(sp4b,msp4b,msp4b+incrStatic,5,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp4b=resize_1D_double(bl_mom_sp4b,msp4b,msp4b+incrStatic);
                bl_mom_sp4=resize_1D_double(bl_mom_sp4,msp4,msp4+incrStatic);
                msp4=msp4+incrStatic;
            }
            msp4b=msp4b+incrStatic;
        }
        sp4b[nsp4b[f]][0] = n0;
        sp4b[nsp4b[f]][1] = n1;
        sp4b[nsp4b[f]][2] = n2;
        sp4b[nsp4b[f]][3] = n3;
        sp4b[nsp4b[f]][4] = cp[0];

        mem_sp4b[n0][nmem_sp4b[n0]]=nsp4b[f];       
        nmem_sp4b[n0]++;        
        if (nmem_sp4b[n0] >= mmem_sp4b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4b[binAcnt]=resize_1D_int(mem_sp4b[binAcnt],mmem_sp4b,mmem_sp4b+incrClustPerPart);
            }
            mmem_sp4b=mmem_sp4b+incrClustPerPart;
        }
        
        mem_sp4b[n1][nmem_sp4b[n1]]=nsp4b[f];       
        nmem_sp4b[n1]++;        
        if (nmem_sp4b[n1] >= mmem_sp4b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4b[binAcnt]=resize_1D_int(mem_sp4b[binAcnt],mmem_sp4b,mmem_sp4b+incrClustPerPart);
            }
            mmem_sp4b=mmem_sp4b+incrClustPerPart;
        }
        
        mem_sp4b[n2][nmem_sp4b[n2]]=nsp4b[f];       
        nmem_sp4b[n2]++;        
        if (nmem_sp4b[n2] >= mmem_sp4b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4b[binAcnt]=resize_1D_int(mem_sp4b[binAcnt],mmem_sp4b,mmem_sp4b+incrClustPerPart);
            }
            mmem_sp4b=mmem_sp4b+incrClustPerPart;
        }
        
        mem_sp4b[n3][nmem_sp4b[n3]]=nsp4b[f];       
        nmem_sp4b[n3]++;        
        if (nmem_sp4b[n3] >= mmem_sp4b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4b[binAcnt]=resize_1D_int(mem_sp4b[binAcnt],mmem_sp4b,mmem_sp4b+incrClustPerPart);
            }
            mmem_sp4b=mmem_sp4b+incrClustPerPart;
        }
        
        mem_sp4b[cp[0]][nmem_sp4b[cp[0]]]=nsp4b[f]; 
        nmem_sp4b[cp[0]]++;     
        if (nmem_sp4b[cp[0]] >= mmem_sp4b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4b[binAcnt]=resize_1D_int(mem_sp4b[binAcnt],mmem_sp4b,mmem_sp4b+incrClustPerPart);
            }
            mmem_sp4b=mmem_sp4b+incrClustPerPart;
        }
        
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4b,&BLDistroNoSamplessp4b);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4b,&BLDistroNoSamplessp4b);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4b,&BLDistroNoSamplessp4b);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4b,&BLDistroNoSamplessp4b);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)],BLDistrosp4b,&BLDistroNoSamplessp4b);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)],BLDistrosp4b,&BLDistroNoSamplessp4b);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)],BLDistrosp4b,&BLDistroNoSamplessp4b);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)],BLDistrosp4b,&BLDistroNoSamplessp4b);
        }

        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4b)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4b);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4b*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4b*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4b*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4b*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4b)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4b);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4b*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4b*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4b*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4b*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4b)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4b);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4b*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4b*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4b*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4b*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n3]==1) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4b)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4b);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n3]==2) {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4b*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4b*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4b[nsp4b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4b*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4b*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB);
            }
            
            for (binAcnt=0; binAcnt<4; binAcnt++) {
                if (rtype[cp[0]]==1 && rtype[sp4b[nsp4b[f]][binAcnt]]==1) {
                    bl_mom_sp4b[nsp4b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4b[nsp4b[f]][binAcnt])]-gsbl_sp4b)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4b[nsp4b[f]][binAcnt])]-gsbl_sp4b);
                }
                else if (rtype[cp[0]]==2 && rtype[sp4b[nsp4b[f]][binAcnt]]==2) {
                    bl_mom_sp4b[nsp4b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4b[nsp4b[f]][binAcnt])]-gsbl_sp4b*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4b[nsp4b[f]][binAcnt])]-gsbl_sp4b*sigma_BB);
                }
                else {
                    bl_mom_sp4b[nsp4b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4b[nsp4b[f]][binAcnt])]-gsbl_sp4b*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4b[nsp4b[f]][binAcnt])]-gsbl_sp4b*sigma_AB);
                }
            }
            
            bl_mom_sp4b[nsp4b[f]]/=8.0;
            bl_mom_sp4[nsp4[f]]/=4.0;
        }
        
        ++nsp4b[f];
    }
    else if (type==2 && dosp4c==1) {
        if (nsp4c[f] == msp4c) { 
            sp4c=resize_2D_int(sp4c,msp4c,msp4c+incrStatic,6,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp4c=resize_1D_double(bl_mom_sp4c,msp4c,msp4c+incrStatic);
                bl_mom_sp4=resize_1D_double(bl_mom_sp4,msp4,msp4+incrStatic);
                msp4=msp4+incrStatic;
            }
            msp4c=msp4c+incrStatic;
        }
        sp4c[nsp4c[f]][0] = n0;
        sp4c[nsp4c[f]][1] = n1;
        sp4c[nsp4c[f]][2] = n2;
        sp4c[nsp4c[f]][3] = n3; 
        if (cp[0]<cp[1]) {
            sp4c[nsp4c[f]][4] = cp[0];
            sp4c[nsp4c[f]][5] = cp[1];
        }
        else {
            sp4c[nsp4c[f]][4] = cp[1];
            sp4c[nsp4c[f]][5] = cp[0];
        }
        mem_sp4c[n0][nmem_sp4c[n0]]=nsp4c[f];       
        nmem_sp4c[n0]++;        
        if (nmem_sp4c[n0] >= mmem_sp4c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4c[binAcnt]=resize_1D_int(mem_sp4c[binAcnt],mmem_sp4c,mmem_sp4c+incrClustPerPart);
            }
            mmem_sp4c=mmem_sp4c+incrClustPerPart;
        }
        
        mem_sp4c[n1][nmem_sp4c[n1]]=nsp4c[f];       
        nmem_sp4c[n1]++;        
        if (nmem_sp4c[n1] >= mmem_sp4c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4c[binAcnt]=resize_1D_int(mem_sp4c[binAcnt],mmem_sp4c,mmem_sp4c+incrClustPerPart);
            }
            mmem_sp4c=mmem_sp4c+incrClustPerPart;
        }
        
        mem_sp4c[n2][nmem_sp4c[n2]]=nsp4c[f];       
        nmem_sp4c[n2]++;        
        if (nmem_sp4c[n2] >= mmem_sp4c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4c[binAcnt]=resize_1D_int(mem_sp4c[binAcnt],mmem_sp4c,mmem_sp4c+incrClustPerPart);
            }
            mmem_sp4c=mmem_sp4c+incrClustPerPart;
        }
        
        mem_sp4c[n3][nmem_sp4c[n3]]=nsp4c[f];       
        nmem_sp4c[n3]++;        
        if (nmem_sp4c[n3] >= mmem_sp4c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4c[binAcnt]=resize_1D_int(mem_sp4c[binAcnt],mmem_sp4c,mmem_sp4c+incrClustPerPart);
            }
            mmem_sp4c=mmem_sp4c+incrClustPerPart;
        }
        
        mem_sp4c[cp[0]][nmem_sp4c[cp[0]]]=nsp4c[f]; 
        nmem_sp4c[cp[0]]++;     
        if (nmem_sp4c[cp[0]] >= mmem_sp4c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4c[binAcnt]=resize_1D_int(mem_sp4c[binAcnt],mmem_sp4c,mmem_sp4c+incrClustPerPart);
            }
            mmem_sp4c=mmem_sp4c+incrClustPerPart;
        }
        
        mem_sp4c[cp[1]][nmem_sp4c[cp[1]]]=nsp4c[f]; 
        nmem_sp4c[cp[1]]++;     
        if (nmem_sp4c[cp[1]] >= mmem_sp4c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp4c[binAcnt]=resize_1D_int(mem_sp4c[binAcnt],mmem_sp4c,mmem_sp4c+incrClustPerPart);
            }
            mmem_sp4c=mmem_sp4c+incrClustPerPart;
        }
        
        if (Bonds_BondCheck(sp4c[nsp4c[f]][4],sp4c[nsp4c[f]][5])==1) nsp4c_spindlebonds[f]++;
        
        for (i=0;i<6;i++) trial[i]=sp4c[nsp4c[f]][i];
        quickSort(&trial[0],6);
        flg=0;  // check trial cluster not already found
        for (i=0; i<n6A[f]; ++i) {
            for (j=0; j<6; ++j) {
                if (trial[j]!=hc6A[i][j]) break;
            }   
            if (j==6) {
                flg=1;
                break;
            }
        }
        if (flg==0) {
            if (n6A[f] == m6A) { 
                hc6A=resize_2D_int(hc6A,m6A,m6A+incrStatic,6,-1);
                if (doClusBLDeviation==1) {
                    bl_mom_6A=resize_1D_double(bl_mom_6A,m6A,m6A+incrStatic);
                }
                m6A=m6A+incrStatic;
            }
            for (i=0; i<6; ++i) hc6A[n6A[f]][i]=trial[i];
            
            if (Bonds_BondCheck(cp[0],cp[1])==1) n6A_spindlebonds[f]++;
            
            if (doClusBLDistros==1) {
                Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistro6A,&BLDistroNoSamples6A);
                
                Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,cp[0])],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,cp[0])],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,cp[0])],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n3][Bonds_cnb_j(n3,cp[0])],BLDistro6A,&BLDistroNoSamples6A);
                
                Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,cp[1])],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,cp[1])],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,cp[1])],BLDistro6A,&BLDistroNoSamples6A);
                Bonds_TickBLDistro(bondlengths[n3][Bonds_cnb_j(n3,cp[1])],BLDistro6A,&BLDistroNoSamples6A);
                
                if (Bonds_BondCheck(cp[0],cp[1])==1) Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],cp[1])],BLDistro6A,&BLDistroNoSamples6A);
            }
            
            if (doClusBLDeviation==1) {
                if (rtype[n0]==1 && rtype[n1]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_6A)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_6A);
                }
                else if (rtype[n0]==2 && rtype[n1]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_6A*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_6A*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[n2]==1 && rtype[n1]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_6A)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_6A);
                }
                else if (rtype[n2]==2 && rtype[n1]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_6A*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_6A*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[n2]==1 && rtype[n3]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_6A)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_6A);
                }
                else if (rtype[n2]==2 && rtype[n3]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_6A*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_6A*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[n0]==1 && rtype[n3]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_6A)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_6A);
                }
                else if (rtype[n0]==2 && rtype[n3]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_6A*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_6A*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[cp[0]]==1 && rtype[n0]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_6A)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_6A);
                }
                else if (rtype[cp[0]]==2 && rtype[n0]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_6A*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_6A*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)]-gsbl_6A*sigma_AB);
                }
                    
                if (rtype[cp[0]]==1 && rtype[n1]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_6A)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_6A);
                }
                else if (rtype[cp[0]]==2 && rtype[n1]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_6A*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_6A*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[cp[0]]==1 && rtype[n2]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_6A)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_6A);
                }
                else if (rtype[cp[0]]==2 && rtype[n2]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_6A*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_6A*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[cp[0]]==1 && rtype[n3]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)]-gsbl_6A)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)]-gsbl_6A);
                }
                else if (rtype[cp[0]]==2 && rtype[n3]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)]-gsbl_6A*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)]-gsbl_6A*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[cp[1]]==1 && rtype[n0]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_6A)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_6A);
                }
                else if (rtype[cp[1]]==2 && rtype[n0]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_6A*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_6A*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)]-gsbl_6A*sigma_AB);
                }
                    
                if (rtype[cp[1]]==1 && rtype[n1]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_6A)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_6A);
                }
                else if (rtype[cp[1]]==2 && rtype[n1]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_6A*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_6A*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[cp[1]]==1 && rtype[n2]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_6A)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_6A);
                }
                else if (rtype[cp[1]]==2 && rtype[n2]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_6A*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_6A*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)]-gsbl_6A*sigma_AB);
                }
                
                if (rtype[cp[1]]==1 && rtype[n3]==1) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n3)]-gsbl_6A)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n3)]-gsbl_6A);
                }
                else if (rtype[cp[1]]==2 && rtype[n3]==2) {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n3)]-gsbl_6A*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n3)]-gsbl_6A*sigma_BB);
                }
                else {
                    bl_mom_6A[n6A[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n3)]-gsbl_6A*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n3)]-gsbl_6A*sigma_AB);
                }
                
                bl_mom_6A[n6A[f]]/=12.0;
            }
            
            ++n6A[f];
        }
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n0)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n1)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n2)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],n3)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            
            Bonds_TickBLDistro(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n0)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n1)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n2)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            Bonds_TickBLDistro(bondlengths[cp[1]][Bonds_cnb_j(cp[1],n3)],BLDistrosp4c,&BLDistroNoSamplessp4c);
            
            if (Bonds_BondCheck(cp[0],cp[1])==1) Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],cp[1])],BLDistrosp4c,&BLDistroNoSamplessp4c);
        }
            
        // hc6A key: (SP4_1, SP4_2, SP4_3, SP4_4, s1, s2)

        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4c)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4c);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4c*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4c*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4c*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4c*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4c)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4c);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4c*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4c*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4c*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4c*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4c)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4c);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4c*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4c*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4c*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4c*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n3]==1) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4c)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4c);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n3]==2) {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4c*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4c*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4c[nsp4c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4c*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4c*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB);
            }
            
            for (binAcnt=0; binAcnt<4; binAcnt++) {
                if (rtype[cp[0]]==1 && rtype[sp4c[nsp4c[f]][binAcnt]]==1) {
                    bl_mom_sp4c[nsp4c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c);
                }
                else if (rtype[cp[0]]==2 && rtype[sp4c[nsp4c[f]][binAcnt]]==2) {
                    bl_mom_sp4c[nsp4c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_BB);
                }
                else {
                    bl_mom_sp4c[nsp4c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_AB);
                }
                
                if (rtype[cp[1]]==1 && rtype[sp4c[nsp4c[f]][binAcnt]]==1) {
                    bl_mom_sp4c[nsp4c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c);
                }
                else if (rtype[cp[1]]==2 && rtype[sp4c[nsp4c[f]][binAcnt]]==2) {
                    bl_mom_sp4c[nsp4c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_BB);
                }
                else {
                    bl_mom_sp4c[nsp4c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp4c[nsp4c[f]][binAcnt])]-gsbl_sp4c*sigma_AB);
                }
            }
            
            bl_mom_sp4c[nsp4c[f]]/=12.0;
            bl_mom_sp4[nsp4[f]]/=4.0;
        }
        
        ++nsp4c[f];
    }
    else if (dosp4a==1) {
        if (nsp4a[f] == msp4a) { 
            sp4a=resize_2D_int(sp4a,msp4a,msp4a+incrStatic,4,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp4a=resize_1D_double(bl_mom_sp4a,msp4a,msp4a+incrStatic);
                bl_mom_sp4=resize_1D_double(bl_mom_sp4,msp4,msp4+incrStatic);
                msp4=msp4+incrStatic;
            }
            msp4a=msp4a+incrStatic;
        }
        sp4a[nsp4a[f]][0] = n0;
        sp4a[nsp4a[f]][1] = n1;
        sp4a[nsp4a[f]][2] = n2;
        sp4a[nsp4a[f]][3] = n3;
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4a,&BLDistroNoSamplessp4a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n3)],BLDistrosp4,&BLDistroNoSamplessp4);
        }

        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp4*sigma_AB);
            }
            
            if (rtype[n0]==1 && rtype[n3]==1) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4);
            }
            else if (rtype[n0]==2 && rtype[n3]==2) {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_BB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_BB);
            }
            else {
                bl_mom_sp4a[nsp4a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4a*sigma_AB);
                bl_mom_sp4[nsp4[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n3)]-gsbl_sp4*sigma_AB);
            }
            
            bl_mom_sp4a[nsp4a[f]]/=4.0;
            bl_mom_sp4[nsp4[f]]/=4.0;
        }
        
        ++nsp4a[f];
        ++nsp4_excess_spindles[f];
    }
    
    ++nsp4[f];
}

void Rings_aSP5(int f, int n0, int n1, int n2, int n3, int n4) {    // Take {n0,n1,n2,n3,n4}, check SP5 ring and if so detect SP5a/b/c cluster
    int i, j;
    int type = 0;
    int cp[2];  // common spindles - particles bonded to all members of three membered ring
    int bcheck;
    int binAcnt;

    cp[0]=cp[1]=-1;
    for (i=0; i<cnb[n0]; ++i) {
        j = bNums[n0][i];
        bcheck = j == n1 || j == n4;
        if (bcheck) continue;
        bcheck = Bonds_BondCheck(n1,j)==1 && Bonds_BondCheck(n2,j)==1 && Bonds_BondCheck(n3,j)==1 && Bonds_BondCheck(n4,j)==1;
        if (bcheck) {
            if (type<2) {
                cp[type] = j;
                type++;
            }
            else type++;
        }
    }
    
    if (maxto5<type) maxto5=type;
    
    if (type==0 && dosp5a==1) { // Now store ring
        if (nsp5a[f] == msp5a) { 
            sp5a=resize_2D_int(sp5a,msp5a,msp5a+incrStatic,5,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp5a=resize_1D_double(bl_mom_sp5a,msp5a,msp5a+incrStatic);
                bl_mom_sp5=resize_1D_double(bl_mom_sp5,msp5,msp5+incrStatic);
                msp5=msp5+incrStatic;
            }
            msp5a=msp5a+incrStatic;
        }
        sp5a[nsp5a[f]][0] = n0;
        sp5a[nsp5a[f]][1] = n1;
        sp5a[nsp5a[f]][2] = n2;
        sp5a[nsp5a[f]][3] = n3;
        sp5a[nsp5a[f]][4] = n4;

        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5,&BLDistroNoSamplessp5);
        }
        
        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n3]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n3]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n0]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n0]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB);
            }
            
            bl_mom_sp5a[nsp5a[f]]/=5.0;
            bl_mom_sp5[nsp5[f]]/=5.0;
        }
        
        ++nsp5a[f];
    }
    else if (type==1 && dosp5b==1) {
        if (nsp5b[f] == msp5b) { 
            sp5b=resize_2D_int(sp5b,msp5b,msp5b+incrStatic,6,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp5b=resize_1D_double(bl_mom_sp5b,msp5b,msp5b+incrStatic);
                bl_mom_sp5=resize_1D_double(bl_mom_sp5,msp5,msp5+incrStatic);
                msp5=msp5+incrStatic;
            }
            msp5b=msp5b+incrStatic;
        }
        sp5b[nsp5b[f]][0] = n0;
        sp5b[nsp5b[f]][1] = n1;
        sp5b[nsp5b[f]][2] = n2;
        sp5b[nsp5b[f]][3] = n3;
        sp5b[nsp5b[f]][4] = n4;
        sp5b[nsp5b[f]][5] = cp[0];

        mem_sp5b[n0][nmem_sp5b[n0]]=nsp5b[f];       
        nmem_sp5b[n0]++;        
        if (nmem_sp5b[n0] >= mmem_sp5b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5b[binAcnt]=resize_1D_int(mem_sp5b[binAcnt],mmem_sp5b,mmem_sp5b+incrClustPerPart);
            }
            mmem_sp5b=mmem_sp5b+incrClustPerPart;
        }
        
        mem_sp5b[n1][nmem_sp5b[n1]]=nsp5b[f];       
        nmem_sp5b[n1]++;        
        if (nmem_sp5b[n1] >= mmem_sp5b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5b[binAcnt]=resize_1D_int(mem_sp5b[binAcnt],mmem_sp5b,mmem_sp5b+incrClustPerPart);
            }
            mmem_sp5b=mmem_sp5b+incrClustPerPart;
        }
        
        mem_sp5b[n2][nmem_sp5b[n2]]=nsp5b[f];       
        nmem_sp5b[n2]++;        
        if (nmem_sp5b[n2] >= mmem_sp5b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5b[binAcnt]=resize_1D_int(mem_sp5b[binAcnt],mmem_sp5b,mmem_sp5b+incrClustPerPart);
            }
            mmem_sp5b=mmem_sp5b+incrClustPerPart;
        }
        
        mem_sp5b[n3][nmem_sp5b[n3]]=nsp5b[f];       
        nmem_sp5b[n3]++;        
        if (nmem_sp5b[n3] >= mmem_sp5b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5b[binAcnt]=resize_1D_int(mem_sp5b[binAcnt],mmem_sp5b,mmem_sp5b+incrClustPerPart);
            }
            mmem_sp5b=mmem_sp5b+incrClustPerPart;
        }
        
        mem_sp5b[n4][nmem_sp5b[n4]]=nsp5b[f];       
        nmem_sp5b[n4]++;        
        if (nmem_sp5b[n4] >= mmem_sp5b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5b[binAcnt]=resize_1D_int(mem_sp5b[binAcnt],mmem_sp5b,mmem_sp5b+incrClustPerPart);
            }
            mmem_sp5b=mmem_sp5b+incrClustPerPart;
        }
        
        mem_sp5b[cp[0]][nmem_sp5b[cp[0]]]=nsp5b[f]; 
        nmem_sp5b[cp[0]]++;     
        if (nmem_sp5b[cp[0]] >= mmem_sp5b) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5b[binAcnt]=resize_1D_int(mem_sp5b[binAcnt],mmem_sp5b,mmem_sp5b+incrClustPerPart);
            }
            mmem_sp5b=mmem_sp5b+incrClustPerPart;
        }
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,cp[0])],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,cp[0])],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,cp[0])],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n3][Bonds_cnb_j(n3,cp[0])],BLDistrosp5b,&BLDistroNoSamplessp5b);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,cp[0])],BLDistrosp5b,&BLDistroNoSamplessp5b);
        }
        
        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5b)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5b);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5b*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5b*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5b*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5b*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5b)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5b);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5b*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5b*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5b*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5b*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5b)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5b);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5b*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5b*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5b*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5b*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n3]==1) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5b)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5b);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n3]==2) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5b*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5b*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5b*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5b*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n0]==1) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5b)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5b);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n0]==2) {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5b*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5b*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5b[nsp5b[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5b*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5b*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB);
            }
            
            for (binAcnt=0; binAcnt<5; binAcnt++) {
                if (rtype[cp[0]]==1 && rtype[sp5b[nsp5b[f]][binAcnt]]==1) {
                    bl_mom_sp5b[nsp5b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5b[nsp5b[f]][binAcnt])]-gsbl_sp5b)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5b[nsp5b[f]][binAcnt])]-gsbl_sp5b);
                }
                else if (rtype[cp[0]]==2 && rtype[sp5b[nsp5b[f]][binAcnt]]==2) {
                    bl_mom_sp5b[nsp5b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5b[nsp5b[f]][binAcnt])]-gsbl_sp5b*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5b[nsp5b[f]][binAcnt])]-gsbl_sp5b*sigma_BB);
                }
                else {
                    bl_mom_sp5b[nsp5b[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5b[nsp5b[f]][binAcnt])]-gsbl_sp5b*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5b[nsp5b[f]][binAcnt])]-gsbl_sp5b*sigma_AB);
                }
            }
            
            bl_mom_sp5b[nsp5b[f]]/=10.0;
            bl_mom_sp5[nsp5[f]]/=5.0;
        }
        
        ++nsp5b[f];
    }
    else if (type==2 && dosp5c==1) {
        if (nsp5c[f] == msp5c) { 
            sp5c=resize_2D_int(sp5c,msp5c,msp5c+incrStatic,7,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp5c=resize_1D_double(bl_mom_sp5c,msp5c,msp5c+incrStatic);
                bl_mom_sp5=resize_1D_double(bl_mom_sp5,msp5,msp5+incrStatic);
                msp5=msp5+incrStatic;
            }
            msp5c=msp5c+incrStatic;
        }
        sp5c[nsp5c[f]][0] = n0;
        sp5c[nsp5c[f]][1] = n1;
        sp5c[nsp5c[f]][2] = n2;
        sp5c[nsp5c[f]][3] = n3; 
        sp5c[nsp5c[f]][4] = n4; 
        if (cp[0]<cp[1]) {
            sp5c[nsp5c[f]][5] = cp[0];
            sp5c[nsp5c[f]][6] = cp[1];
        }
        else {
            sp5c[nsp5c[f]][5] = cp[1];
            sp5c[nsp5c[f]][6] = cp[0];
        }

        mem_sp5c[n0][nmem_sp5c[n0]]=nsp5c[f];       
        nmem_sp5c[n0]++;        
        if (nmem_sp5c[n0] >= mmem_sp5c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5c[binAcnt]=resize_1D_int(mem_sp5c[binAcnt],mmem_sp5c,mmem_sp5c+incrClustPerPart);
            }
            mmem_sp5c=mmem_sp5c+incrClustPerPart;
        }
        
        mem_sp5c[n1][nmem_sp5c[n1]]=nsp5c[f];       
        nmem_sp5c[n1]++;        
        if (nmem_sp5c[n1] >= mmem_sp5c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5c[binAcnt]=resize_1D_int(mem_sp5c[binAcnt],mmem_sp5c,mmem_sp5c+incrClustPerPart);
            }
            mmem_sp5c=mmem_sp5c+incrClustPerPart;
        }
        
        mem_sp5c[n2][nmem_sp5c[n2]]=nsp5c[f];       
        nmem_sp5c[n2]++;        
        if (nmem_sp5c[n2] >= mmem_sp5c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5c[binAcnt]=resize_1D_int(mem_sp5c[binAcnt],mmem_sp5c,mmem_sp5c+incrClustPerPart);
            }
            mmem_sp5c=mmem_sp5c+incrClustPerPart;
        }
        
        mem_sp5c[n3][nmem_sp5c[n3]]=nsp5c[f];       
        nmem_sp5c[n3]++;        
        if (nmem_sp5c[n3] >= mmem_sp5c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5c[binAcnt]=resize_1D_int(mem_sp5c[binAcnt],mmem_sp5c,mmem_sp5c+incrClustPerPart);
            }
            mmem_sp5c=mmem_sp5c+incrClustPerPart;
        }
        
        mem_sp5c[n4][nmem_sp5c[n4]]=nsp5c[f];       
        nmem_sp5c[n4]++;        
        if (nmem_sp5c[n4] >= mmem_sp5c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5c[binAcnt]=resize_1D_int(mem_sp5c[binAcnt],mmem_sp5c,mmem_sp5c+incrClustPerPart);
            }
            mmem_sp5c=mmem_sp5c+incrClustPerPart;
        }
        
        mem_sp5c[cp[0]][nmem_sp5c[cp[0]]]=nsp5c[f]; 
        nmem_sp5c[cp[0]]++;     
        if (nmem_sp5c[cp[0]] >= mmem_sp5c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5c[binAcnt]=resize_1D_int(mem_sp5c[binAcnt],mmem_sp5c,mmem_sp5c+incrClustPerPart);
            }
            mmem_sp5c=mmem_sp5c+incrClustPerPart;
        }
        
        mem_sp5c[cp[1]][nmem_sp5c[cp[1]]]=nsp5c[f]; 
        nmem_sp5c[cp[1]]++;     
        if (nmem_sp5c[cp[1]] >= mmem_sp5c) { 
            for (binAcnt=0; binAcnt<N; binAcnt++) {
                mem_sp5c[binAcnt]=resize_1D_int(mem_sp5c[binAcnt],mmem_sp5c,mmem_sp5c+incrClustPerPart);
            }
            mmem_sp5c=mmem_sp5c+incrClustPerPart;
        }
        
        
        if (Bonds_BondCheck(sp5c[nsp5c[f]][5],sp5c[nsp5c[f]][6])==1) nsp5c_spindlebonds[f]++;
        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,cp[0])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,cp[0])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,cp[0])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n3][Bonds_cnb_j(n3,cp[0])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,cp[0])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,cp[1])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n1][Bonds_cnb_j(n1,cp[1])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,cp[1])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n3][Bonds_cnb_j(n3,cp[1])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,cp[1])],BLDistrosp5c,&BLDistroNoSamplessp5c);
            
            if (Bonds_BondCheck(cp[0],cp[1])==1) Bonds_TickBLDistro(bondlengths[cp[0]][Bonds_cnb_j(cp[0],cp[1])],BLDistrosp5c,&BLDistroNoSamplessp5c);
        }
        
        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5c)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5c);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5c*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5c*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5c*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5c*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5c)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5c);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5c*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5c*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5c*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5c*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5c)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5c);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5c*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5c*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5c*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5c*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n3]==1) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5c)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5c);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n3]==2) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5c*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5c*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5c*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5c*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n0]==1) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5c)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5c);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n0]==2) {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5c*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5c*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5c[nsp5c[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5c*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5c*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB);
            }
            
            for (binAcnt=0; binAcnt<5; binAcnt++) {
                if (rtype[cp[0]]==1 && rtype[sp5c[nsp5c[f]][binAcnt]]==1) {
                    bl_mom_sp5c[nsp5c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c);
                }
                else if (rtype[cp[0]]==2 && rtype[sp5c[nsp5c[f]][binAcnt]]==2) {
                    bl_mom_sp5c[nsp5c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_BB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_BB);
                }
                else {
                    bl_mom_sp5c[nsp5c[f]]+=(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_AB)*(bondlengths[cp[0]][Bonds_cnb_j(cp[0],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_AB);
                }
                
                if (rtype[cp[1]]==1 && rtype[sp5c[nsp5c[f]][binAcnt]]==1) {
                    bl_mom_sp5c[nsp5c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c);
                }
                else if (rtype[cp[1]]==2 && rtype[sp5c[nsp5c[f]][binAcnt]]==2) {
                    bl_mom_sp5c[nsp5c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_BB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_BB);
                }
                else {
                    bl_mom_sp5c[nsp5c[f]]+=(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_AB)*(bondlengths[cp[1]][Bonds_cnb_j(cp[1],sp5c[nsp5c[f]][binAcnt])]-gsbl_sp5c*sigma_AB);
                }
            }
            
            bl_mom_sp5c[nsp5c[f]]/=15.0;
            bl_mom_sp5[nsp5[f]]/=5.0;
        }
        
        ++nsp5c[f];
    }
    else if (dosp5a==1) {   // Now store ring
        if (nsp5a[f] == msp5a) { 
            sp5a=resize_2D_int(sp5a,msp5a,msp5a+incrStatic,5,-1);
            if (doClusBLDeviation==1) {
                bl_mom_sp5a=resize_1D_double(bl_mom_sp5a,msp5a,msp5a+incrStatic);
                bl_mom_sp5=resize_1D_double(bl_mom_sp5,msp5,msp5+incrStatic);
                msp5=msp5+incrStatic;
            }
            msp5a=msp5a+incrStatic;
        }
        sp5a[nsp5a[f]][0] = n0;
        sp5a[nsp5a[f]][1] = n1;
        sp5a[nsp5a[f]][2] = n2;
        sp5a[nsp5a[f]][3] = n3;
        sp5a[nsp5a[f]][4] = n4;

        
        if (doClusBLDistros==1) {
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n0][Bonds_cnb_j(n0,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n1)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n2][Bonds_cnb_j(n2,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n3)],BLDistrosp5,&BLDistroNoSamplessp5);
            
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5a,&BLDistroNoSamplessp5a);
            Bonds_TickBLDistro(bondlengths[n4][Bonds_cnb_j(n4,n0)],BLDistrosp5,&BLDistroNoSamplessp5);
        }

        if (doClusBLDeviation==1) {
            if (rtype[n0]==1 && rtype[n1]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5);
            }
            else if (rtype[n0]==2 && rtype[n1]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n0][Bonds_cnb_j(n0,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n1]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n1]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n1)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n2]==1 && rtype[n3]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5);
            }
            else if (rtype[n2]==2 && rtype[n3]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n2][Bonds_cnb_j(n2,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n3]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n3]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n3)]-gsbl_sp5*sigma_AB);
            }
            
            if (rtype[n4]==1 && rtype[n0]==1) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5);
            }
            else if (rtype[n4]==2 && rtype[n0]==2) {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_BB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_BB);
            }
            else {
                bl_mom_sp5a[nsp5a[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5a*sigma_AB);
                bl_mom_sp5[nsp5[f]]+=(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB)*(bondlengths[n4][Bonds_cnb_j(n4,n0)]-gsbl_sp5*sigma_AB);
            }
            
            bl_mom_sp5a[nsp5a[f]]/=5.0;
            bl_mom_sp5[nsp5[f]]/=5.0;
        }
        
        ++nsp5a[f];
        ++nsp5_excess_spindles[f];
    }
    
    ++nsp5[f];
}

void Rings_setSP3c(int f) { // store cluster 5A D3h from Bonds_aSP3
    int i;
    char *ach, errMsg[1000];

    ach=malloc(N*sizeof(char)); if (ach==NULL) { sprintf(errMsg,"Rings_setSP3c(): ach[] malloc out of memory\n");   Error(errMsg); }
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp3a[f]; i++) {
        if (ach[sp3a[i][0]] == 'C') ach[sp3a[i][0]] = 'B';
        if (ach[sp3a[i][1]] == 'C') ach[sp3a[i][1]] = 'B';
        if (ach[sp3a[i][2]] == 'C') ach[sp3a[i][2]] = 'B';
    }
    for (i=0; i<N; ++i) ssp3a[i]=ach[i];
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp3b[f]; i++) {
        if (ach[sp3b[i][0]] == 'C') ach[sp3b[i][0]] = 'B';
        if (ach[sp3b[i][1]] == 'C') ach[sp3b[i][1]] = 'B';
        if (ach[sp3b[i][2]] == 'C') ach[sp3b[i][2]] = 'B';
        ach[sp3b[i][3]] = 'O';
    }
    for (i=0; i<N; ++i) ssp3b[i]=ach[i];
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp3c[f]; i++) {
        if (ach[sp3c[i][0]] == 'C') ach[sp3c[i][0]] = 'B';
        if (ach[sp3c[i][1]] == 'C') ach[sp3c[i][1]] = 'B';
        if (ach[sp3c[i][2]] == 'C') ach[sp3c[i][2]] = 'B';
        ach[sp3c[i][3]] = 'O';
        ach[sp3c[i][4]] = 'O';
    }
    for (i=0; i<N; ++i) s5A[i]=ach[i];
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp3a[f]; i++) {
        if (ach[sp3a[i][0]] == 'C') ach[sp3a[i][0]] = 'B';
        if (ach[sp3a[i][1]] == 'C') ach[sp3a[i][1]] = 'B';
        if (ach[sp3a[i][2]] == 'C') ach[sp3a[i][2]] = 'B';
    }
    for (i=0; i<nsp3b[f]; i++) {
        if (ach[sp3b[i][0]] == 'C') ach[sp3b[i][0]] = 'B';
        if (ach[sp3b[i][1]] == 'C') ach[sp3b[i][1]] = 'B';
        if (ach[sp3b[i][2]] == 'C') ach[sp3b[i][2]] = 'B';
    }
    for (i=0; i<nsp3c[f]; i++) {
        if (ach[sp3c[i][0]] == 'C') ach[sp3c[i][0]] = 'B';
        if (ach[sp3c[i][1]] == 'C') ach[sp3c[i][1]] = 'B';
        if (ach[sp3c[i][2]] == 'C') ach[sp3c[i][2]] = 'B';
    }
    for (i=0; i<N; ++i) ssp3[i]=ach[i];

    free(ach);
}

void Rings_setSP4c(int f) { // store cluster 6A Oh from Bonds_aSP4()
    int i;
    char *ach, errMsg[1000];

    ach=malloc(N*sizeof(char)); if (ach==NULL) { sprintf(errMsg,"Rings_setSP4c(): ach[] malloc out of memory\n");   Error(errMsg); }
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp4a[f]; i++) {
        if (ach[sp4a[i][0]] == 'C') ach[sp4a[i][0]] = 'B';
        if (ach[sp4a[i][1]] == 'C') ach[sp4a[i][1]] = 'B';
        if (ach[sp4a[i][2]] == 'C') ach[sp4a[i][2]] = 'B';
        if (ach[sp4a[i][3]] == 'C') ach[sp4a[i][3]] = 'B';
    }
    for (i=0; i<N; ++i) ssp4a[i]=ach[i];
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp4b[f]; i++) {
        if (ach[sp4b[i][0]] == 'C') ach[sp4b[i][0]] = 'B';
        if (ach[sp4b[i][1]] == 'C') ach[sp4b[i][1]] = 'B';
        if (ach[sp4b[i][2]] == 'C') ach[sp4b[i][2]] = 'B';
        if (ach[sp4b[i][3]] == 'C') ach[sp4b[i][3]] = 'B';
        ach[sp4b[i][4]] = 'O';
    }
    for (i=0; i<N; ++i) ssp4b[i]=ach[i];
    
    for (i=0; i<N; ++i) ach[i] = 'C';
    for (i=0; i<nsp4c[f]; ++i) {
        if (ach[sp4c[i][0]] == 'C') ach[sp4c[i][0]] = 'B';
        if (ach[sp4c[i][1]] == 'C') ach[sp4c[i][1]] = 'B';
        if (ach[sp4c[i][2]] == 'C') ach[sp4c[i][2]] = 'B';
        if (ach[sp4c[i][3]] == 'C') ach[sp4c[i][3]] = 'B';
        ach[sp4c[i][4]] = 'O';
        ach[sp4c[i][5]] = 'O';
    }
    for (i=0; i<N; ++i) s6A[i]=ach[i];
    
    for (i=0; i<N; ++i) ach[i] = 'C';
    for (i=0; i<nsp4a[f]; i++) {
        if (ach[sp4a[i][0]] == 'C') ach[sp4a[i][0]] = 'B';
        if (ach[sp4a[i][1]] == 'C') ach[sp4a[i][1]] = 'B';
        if (ach[sp4a[i][2]] == 'C') ach[sp4a[i][2]] = 'B';
        if (ach[sp4a[i][3]] == 'C') ach[sp4a[i][3]] = 'B';
    }
    for (i=0; i<nsp4b[f]; i++) {
        if (ach[sp4b[i][0]] == 'C') ach[sp4b[i][0]] = 'B';
        if (ach[sp4b[i][1]] == 'C') ach[sp4b[i][1]] = 'B';
        if (ach[sp4b[i][2]] == 'C') ach[sp4b[i][2]] = 'B';
        if (ach[sp4b[i][3]] == 'C') ach[sp4b[i][3]] = 'B';
    }
    for (i=0; i<nsp4c[f]; ++i) {
        if (ach[sp4c[i][0]] == 'C') ach[sp4c[i][0]] = 'B';
        if (ach[sp4c[i][1]] == 'C') ach[sp4c[i][1]] = 'B';
        if (ach[sp4c[i][2]] == 'C') ach[sp4c[i][2]] = 'B';
        if (ach[sp4c[i][3]] == 'C') ach[sp4c[i][3]] = 'B';
    }
    for (i=0; i<N; ++i) ssp4[i]=ach[i];

    
    free(ach);
}

void Rings_setSP5c(int f) { // store cluster 7A D5h from Bonds_aSP5()
    int i;
    char *ach, errMsg[1000];

    ach=malloc(N*sizeof(char)); if (ach==NULL) { sprintf(errMsg,"Rings_setSP5c(): ach[] malloc out of memory\n");   Error(errMsg); }
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp5a[f]; i++) {
        if (ach[sp5a[i][0]] == 'C') ach[sp5a[i][0]] = 'B';
        if (ach[sp5a[i][1]] == 'C') ach[sp5a[i][1]] = 'B';
        if (ach[sp5a[i][2]] == 'C') ach[sp5a[i][2]] = 'B';
        if (ach[sp5a[i][3]] == 'C') ach[sp5a[i][3]] = 'B';
        if (ach[sp5a[i][4]] == 'C') ach[sp5a[i][4]] = 'B';
    }
    for (i=0; i<N; ++i) ssp5a[i]=ach[i];
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp5b[f]; i++) {
        if (ach[sp5b[i][0]] == 'C') ach[sp5b[i][0]] = 'B';
        if (ach[sp5b[i][1]] == 'C') ach[sp5b[i][1]] = 'B';
        if (ach[sp5b[i][2]] == 'C') ach[sp5b[i][2]] = 'B';
        if (ach[sp5b[i][3]] == 'C') ach[sp5b[i][3]] = 'B';
        if (ach[sp5b[i][4]] == 'C') ach[sp5b[i][4]] = 'B';
        ach[sp5b[i][5]] = 'O';
    }
    for (i=0; i<N; ++i) ssp5b[i]=ach[i];
    
    for (i=0; i<N; ++i) ach[i] = 'C';
    for (i=0; i<nsp5c[f]; ++i) {
        if (ach[sp5c[i][0]] == 'C') ach[sp5c[i][0]] = 'B';
        if (ach[sp5c[i][1]] == 'C') ach[sp5c[i][1]] = 'B';
        if (ach[sp5c[i][2]] == 'C') ach[sp5c[i][2]] = 'B';
        if (ach[sp5c[i][3]] == 'C') ach[sp5c[i][3]] = 'B';
        if (ach[sp5c[i][4]] == 'C') ach[sp5c[i][4]] = 'B';
        ach[sp5c[i][5]] = 'O';
        ach[sp5c[i][6]] = 'O';
    }
    for (i=0; i<N; ++i) s7A[i]=ach[i];
    
    for (i=0; i<N; i++) ach[i] = 'C';
    for (i=0; i<nsp5a[f]; i++) {
        if (ach[sp5a[i][0]] == 'C') ach[sp5a[i][0]] = 'B';
        if (ach[sp5a[i][1]] == 'C') ach[sp5a[i][1]] = 'B';
        if (ach[sp5a[i][2]] == 'C') ach[sp5a[i][2]] = 'B';
        if (ach[sp5a[i][3]] == 'C') ach[sp5a[i][3]] = 'B';
        if (ach[sp5a[i][4]] == 'C') ach[sp5a[i][4]] = 'B';
    }
    for (i=0; i<nsp5b[f]; i++) {
        if (ach[sp5b[i][0]] == 'C') ach[sp5b[i][0]] = 'B';
        if (ach[sp5b[i][1]] == 'C') ach[sp5b[i][1]] = 'B';
        if (ach[sp5b[i][2]] == 'C') ach[sp5b[i][2]] = 'B';
        if (ach[sp5b[i][3]] == 'C') ach[sp5b[i][3]] = 'B';
        if (ach[sp5b[i][4]] == 'C') ach[sp5b[i][4]] = 'B';
    }
    for (i=0; i<nsp5c[f]; ++i) {
        if (ach[sp5c[i][0]] == 'C') ach[sp5c[i][0]] = 'B';
        if (ach[sp5c[i][1]] == 'C') ach[sp5c[i][1]] = 'B';
        if (ach[sp5c[i][2]] == 'C') ach[sp5c[i][2]] = 'B';
        if (ach[sp5c[i][3]] == 'C') ach[sp5c[i][3]] = 'B';
        if (ach[sp5c[i][4]] == 'C') ach[sp5c[i][4]] = 'B';
    }
    for (i=0; i<N; ++i) ssp5[i]=ach[i];
    
    free(ach);
}