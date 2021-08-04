#include <stdio.h>

#define MODE 1

int Check_RIFF();			//ελενχος αν υπαρχει RIFF
int Check_SizeOfFile();		//ο χορος που καταλαμβαζει το αρχειο(ή μποορει να χρισυμεφσει και για αν βρισκει το μεγεθος 4 bytes)
int Check_WAVE();			//ελενχος αν υπαρχει το WAVE
int Check_fmt();			//ελενχος αν υπαρχει το fmt
int Check_Format_Chunck();		//ελενχος αν υπαρχει το Format Chunck
int Check_Format_WAVE();		//ελενχος αν υπαρχει το Format WAVE
int little_endian_For_2Bytes();	//βρισκει το μεγεθος που εχουν 2 bytes
int MonoStereo();				//ελενχει αν ο ηχος ειναι στερεοφωνικος ή οχι
int SampleRate();				//πόσες τιμές, ανά δευτερόλεπτο,της συνάρτησης του αναλογικού ήχου έχουν καταγραϕεί στο αρχείο.
int BytesPerSec();			//για το πλήθος bytes ανά δευτερόλεπτο ήχου, που είναι καταχωρημένα στο αρχείο.
int BlockAlign();			//για το πλήθος των bytes που απαιτούνται για την καταχώρηση της πληροϕορίας του ήχου σε μία χρονικά στιγμή,για όλα τα κανάλια.
int BitsPerSample();		//για το πλήθος των bits που απαιτούνται για την καταχώρηση της πληροϕορίας του ήχου σε μία χρονικά στιγμή, για ένα μόνο κανάλι.
int Data_chunk();			//4 bytes με τους χαρακτήρες “data”
int SizeOfData();			//συμβολικά SizeOfData, για τον χώρο που καταλαμβάνουν τα δεδομένα του τμήματος data που θα ακολουθήσουν
int SizeOfExtra(int ); 		//μετραει τα bytes μετα τα sizeofdata 

int main(int argc, char const *argv[])
{
	int sizeoffile, tmp, samplerate, bytespersec, blockalign, bitspersample, sizeofdata, sizeofextra;

	//ελενχος αν υπαρχει RIFF
	if (Check_RIFF()){
		fprintf(stderr,"Error! 'RIFF' not found\n");
		return 0;
	}
	
	//ο χορος που καταλαμβαζει το αρχειο
	fprintf(stderr,"size of file: %d\n",(sizeoffile = Check_SizeOfFile()) );
	
	//ελενχος αν υπαρχει το WAVE
	if (Check_WAVE()){
		fprintf(stderr,"Error! 'WAVE' not found\n");
		return 0;
	}

	//ελενχος αν υπαρχει το fmt
	if(Check_fmt()){
		fprintf(stderr,"Error! 'fmt'  not found\n");
		return 0;
	}

	//ελενχος αν υπαρχει το 
	if((tmp = Check_Format_Chunck()) == 16){
		fprintf(stderr,"size of format chunck: %d\n",tmp );
	}
	else{
		fprintf(stderr,"size of format chunck: %d\n",tmp );
		fprintf(stderr,"Error! size of format chunck should be 16\n");
		return 0;
	}

	if((tmp = Check_Format_WAVE()) == 1){
		fprintf(stderr,"WAVE type format: %d\n",tmp );
	}
	else{
		fprintf(stderr,"WAVE type format: %d\n",tmp );
		fprintf(stderr,"Error! WAVE type format should be 1\n");
		return 0;
	}

	tmp = MonoStereo();

	if( (tmp == 1) || (tmp == 2) )
		fprintf(stderr,"mono/stereo: %d\n",tmp );
	else{
		fprintf(stderr,"mono/stereo: %d\n",tmp );
		fprintf(stderr,"Error! mono/stereo should be 1 or 2\n");
		return 0;
	}


	fprintf(stderr,"sample rate: %d\n",(samplerate = SampleRate()));
	
	fprintf(stderr,"bytes/sec: %d\n",(bytespersec = BytesPerSec()));

	fprintf(stderr,"block alignment: %d\n",(blockalign = BlockAlign()));
	if(bytespersec != (samplerate * blockalign)){
		fprintf(stderr,"Error! bytes/second should be sample rate x block alignment\n");
		return 0;
	}

	fprintf(stderr,"bits/sample: %d\n",(bitspersample = BitsPerSample()));
	if ( !((bitspersample == 8) || (bitspersample == 16)) ){
		fprintf(stderr,"Error! bits/sample should be 8 or 16\n");
		return 0;
	}

	if (blockalign != (bitspersample / 8 * tmp) ){
		fprintf(stderr,"Error! block alignment should be bits per sample / 8 x mono/stereo\n");
		return 0;
	}

	if (Data_chunk()){
		fprintf(stderr,"Error! 'data' not found\n");
		return 0;
	}
	
	fprintf(stderr,"size of data chunk: %d\n", (sizeofdata = SizeOfData()));

	sizeofextra = SizeOfExtra(sizeofdata);

	if( !(sizeofextra) ){
		fprintf(stderr,"Error! insufficient data\n");
		return 0;
	}
		
	if ( (36 + sizeofextra + sizeofdata) != sizeoffile)
		fprintf(stderr,"Error! bad file size\n");

	return 0;
}


int Check_RIFF()
{	
	int tmp = Check_SizeOfFile();

	if (tmp == 0x46464952)
		return 0;
	else
		return 1;
}

int Check_SizeOfFile()
{
	int b1, b2, b3, b4, numdiv, nummod, sum = 0;
	
	b1 = getchar();
	b2 = getchar();
	b3 = getchar();
	b4 = getchar();
	
	if (b1 != 0x00){
		numdiv = b1 / 16;
		nummod = b1 % 16;

		sum = sum + (nummod * 1) + (numdiv * 16);
	}

	if(b2 != 0x00){
		numdiv = b2 / 16;
		nummod = b2 % 16;

		sum = sum + (nummod * 256) + (numdiv * 4096);
	}

	if(b3 != 0x00){
		numdiv = b3 / 16;
		nummod = b3 % 16;

		sum = sum + (nummod * 65536) + (numdiv * 1048576);
	}
	
	if(b4 != 0x00){
		numdiv = b4 / 16;
		nummod = b4 % 16;

		sum = sum + (nummod * 16777216) + (numdiv * 268435456);
	}

	return sum;
}

int Check_WAVE()
{
	int tmp = Check_SizeOfFile();

	if (tmp == 0x45564157)
		return 0;
	else
		return 1;	
}


int Check_fmt()
{
	int tmp = Check_SizeOfFile();

	if (tmp == 0x20746d66)
		return 0;
	else
		return 1;
}

int Check_Format_Chunck()
{
	return Check_SizeOfFile();	
}

int Check_Format_WAVE()
{
	return little_endian_For_2Bytes();
}

int little_endian_For_2Bytes()
{
	int b1, b2, numdiv, nummod, sum = 0;
	
	b1 = getchar();
	b2 = getchar();
	
	if (b1 != 0x00){
		numdiv = b1/16;
		nummod = b1%16;

		sum = sum + (nummod * 1) + (numdiv * 16);
	}

	if(b2 != 0x00){
		numdiv = b2 / 16;
		nummod = b2 % 16;

		sum = sum + (nummod * 256) + (numdiv * 4096);
	}

	return sum;
}

int MonoStereo()
{
	return little_endian_For_2Bytes();	
}

int SampleRate()
{
	return Check_SizeOfFile();
}

int BytesPerSec()
{
	return Check_SizeOfFile();	
}

int BlockAlign()
{
	return little_endian_For_2Bytes();	
}

int BitsPerSample()
{
	return little_endian_For_2Bytes();
}

int Data_chunk()
{
	int tmp;

	tmp = Check_SizeOfFile();

	if (tmp == 0x61746164)
		return 0;
	else
		return 1;
}

int SizeOfData()
{
	return Check_SizeOfFile();
}

int SizeOfExtra(int sizeofdata)
{
	int CountDataBytes = 0, CountExtraBytes = 0;
	while ( getchar() != EOF ){
		CountDataBytes++;
		if(CountDataBytes > sizeofdata)
			CountExtraBytes++;
	}
	if ( CountExtraBytes == 0)
		return 0;
	return CountExtraBytes;
}