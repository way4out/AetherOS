static void daw(void){
    page("AETHER DAW / STUDIO");
    int active=0; for(int t=0;t<4;t++)for(int i=0;i<16;i++)if(dawPattern[t][i])active++;
    dawBars=(int)((frameCounter/4)%16);
    iprintf("16-STEP / 4-TRACK LOCAL DAW  BPM %u  STEP %02u\n",save.dawBpm,save.dawStep);
    for(int t=0;t<4;t++){iprintf("T%d ",t+1);for(int i=0;i<16;i++)iprintf("%c",i==save.dawStep?dawPattern[t][i]?'O':'^':dawPattern[t][i]?'#':'.');iprintf(" V%03d %s\n",dawVolume[t],t==dawTrack?"EDIT":"");}
    iprintf("VIEW %s  TRACK %d  ACTIVE STEPS %d  MUTE %d\n",dawView?"MIXER":"SEQUENCER",dawTrack+1,active,dawTrackMute);
    iprintf("OCT %d  SWING %d%%  FX %d  SWING PHASE %d\n",dawOctave,dawSwing,dawFx,dawSwingPhase);
    iprintf("AUDIO PSG/PCM LOCAL  PLAY %s  DSP LINK %s\n",dawPlaying?"RUN":"STOP",dspLink?"ON":"READY");
    footer("UP/DOWN STEP  A TOGGLE NOTE  X PLAY  Y BPM  L/R TRACK  SELECT VIEW  B HOME");
}

static void calculator(void){
    page("AETHER CALCULATOR");
    long long a=calcA,b=calcB,result=calcResult();
    const char *fn[]={"ADD","SUB","MUL","DIV","MOD","PERCENT","SQUARE","CUBE","MAX","MIN","BIT-XOR","Q-NORM"};
    calcError=(calcOp==3&&b==0)||(calcOp==4&&b==0);
    iprintf("A %lld   B %lld   OP %s\n",a,b,fn[calcOp%12]);
    iprintf("RESULT %lld  %s\n",result,calcError?"DIV/0 GUARD":"VALID");
    iprintf("ENTRY %s  SIGN %d  MEMORY %d/%d\n",calcInput?"B":"A",calcSign,calcMemory,calcMemory2);
    iprintf("KEYPAD 0-9  +/-  %  MOD  SQ  CUBE  XOR\n");
    iprintf("[ 7 ][ 8 ][ 9 ][ + ] [ 4 ][ 5 ][ 6 ][ - ]\n");
    iprintf("[ 1 ][ 2 ][ 3 ][ * ] [ 0 ][ = ][ M ][ R ]\n");
    iprintf("Q-MATH PHASE %d  PARITY %d  MOD256 %lld\n",(int)((a*7+b*3)%360),(int)((a^b)&1),(a*b)%256);
    footer("UP/DOWN OP  L/R A/B  A EXEC/M+  X SWAP  Y SIGN  SELECT MEMORY  B HOME");
}


static void animal(void){
    page("ANIMAL AI / ANALYSIS LAB");
    int a=animalPage%15,feature=animalFeature%5;
    int pitch=(a*17+(int)frameCounter)%100, energy=(a*29+(int)(frameCounter/2))%100, rhythm=(a*11+(int)frameCounter)%100;
    int value=feature==0?pitch:feature==1?energy:feature==2?rhythm:feature==3?((pitch+energy)/2):((rhythm+energy)/2);
    animalConfidence=animalAnalyzing?65+(a*3)%31:0;
    animalEvents=(int)((frameCounter/6+a)%128);
    animalOutput=animalAnalyzing?((value+feature*13)%8):0;
    animalHistory[(frameCounter/8)&7]=value;
    iprintf("SPECIES %-8s  ANALYSIS %s  CONF %02d%%\n",animalNames[a],animalAnalyzing?"LIVE":"READY",animalConfidence);
    iprintf("MIC -> FEATURE EXTRACTION -> STATE MODEL -> OUTPUT\n");
    iprintf("PITCH %02d  ENERGY %02d  RHYTHM %02d  EVENTS %03d\n",pitch,energy,rhythm,animalEvents);
    iprintf("FEATURE %-8s VALUE %02d  WINDOW %dms\n",feature==0?"PITCH":feature==1?"ENERGY":feature==2?"RHYTHM":feature==3?"SPECTRUM":"ONSETS",value,64+feature*32);
    iprintf("STATE %s  OUTPUT CLASS %d\n",value<35?"CALM":value<70?"ALERT":"SOCIAL",animalOutput);
    iprintf("HIST ");for(int i=0;i<8;i++)iprintf("%02d ",animalHistory[i]);iprintf("\n");
    iprintf("PHONE INPUT %s  VOCAL OUTPUT %s\n",animalLink?"READY":"LOCAL",animalOutput?"READY":"IDLE");
    iprintf("Classification/feature engine; not literal animal-language translation.\n");
    footer("UP/DOWN SPECIES  L/R FEATURE  A ANALYZE  X PLAY  Y RESET  B HOME");
}


