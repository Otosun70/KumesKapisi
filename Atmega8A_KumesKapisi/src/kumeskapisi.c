/*
 * kumeskapisi.c
 *
 * Created: 13.07.2019 12:04:52
 *  Author: Orhan
 *
 *	Bu proje tavuklar�n bar�nd��� bir k�mesin kap�s�n� g�ne� do�du�unda a�mas�,
 *	g�ne� batt���nda kapatmas� i�in yap�lm��t�r. 
 *
 *	Her g�n sabah ve ak�am hayvanlarla zaman�nda ilgilenme f�rsat�n�n bulunmad��� 
 *	durumlarda hayvanlar�n zarar g�rmesini �nlemek ama�lanm��t�r.
 *	
 *	Proje farkl� ama�lar i�in kullan�labilir.
 *
 */ 
#include "include.h"

char calismaModlari[5]={'D','E','O','S','T'};//Dur,Emniyet,Otomatik,S�k��ma,Test

/* 
	Belirlenen s�re kadar istenen i��k seviyesi devam etti�inde 
	g�nd�z veya gece oldu�una karar veren fonksiyon 
*/  

void gece_gunduz_algilama()
{
	if (isikSeviyesi <= GECE_ISIK_SEVIYESI)
	{
		sayacGece++;
		sayacGunduz=0;
		
		if (sayacGece>(DEGISIM_BEKLEME_SURESI+kapanmaGecikmesi))
		{
			gunduzDurumu=false;
			sayacGece=(DEGISIM_BEKLEME_SURESI+kapanmaGecikmesi+1);
		}
	}
	else if (isikSeviyesi >= GUNDUZ_ISIK_SEVIYESI)
	{
		sayacGunduz++;
		sayacGece=0;

		if (sayacGunduz>DEGISIM_BEKLEME_SURESI)
		{
			gunduzDurumu=true;
			sayacGunduz=DEGISIM_BEKLEME_SURESI+1;
		}
	}
}

void kapiyi_kapat()
{
	if (sayac_motorAdim < KAPANMA_ADIM_SAYISI)
	{
		optik_kapi_emniyeti_enable();	
		optik_Encoder_enable();
		motor1_ileri();
		motor1_yon=true;

	}
	else
	{
		motor1_dur();
		optik_kapi_emniyeti_disable();
		optik_Encoder_disable();
	}
}

void kapiyi_ac()
{
	if (sayac_motorAdim > BASLANGIC_ADIMI)
	{
		optik_Encoder_enable();
		motor1_geri();
		motor1_yon=false;

	}
	else
	{
		motor1_dur();
		optik_Encoder_disable();
	}

}

/*
	Kap� d��ar�dan m�dahale ile a��lmaya veya kapat�lmaya �al���ld���nda 
	bu durumu alg�lay�p kap�y� olmas� gereken duruma getiren fonksiyon.
*/

void kapi_acik_kapali_kontrolu()
{
	if (sayac_motorAdim==BASLANGIC_ADIMI && !durum_switch_acik())
	{
		sayac_motorAdim=sayac_motorAdim+4;
	} 
	else if (sayac_motorAdim==KAPANMA_ADIM_SAYISI && !durum_switch_kapali())
	{
		sayac_motorAdim=KAPANMA_ADIM_SAYISI-4;
	}
}

void kapiyi_otomatik_acma_kapatma()
{
	if(gunduzDurumu)
	{
		kapiyi_ac();
	}
	else
	{
		kapiyi_kapat();
	}
}
/*
	Kap�, donan�mda bulunan siwitch'lere ula�t���nda saya�lar� ve 
	motorun s�k��mas�na kar�� durumlar� ayarlayan fonksiyon 
*/
void switchDurumunaGoreSayacAyarlama()
{
	if (durum_switch_acik())
	{
		sayac_motorAdim=BASLANGIC_ADIMI;
		ledPeriyot=LED_NORMAL_ACIK;
		motor1_sikisma=false;
		sayac_motor1_sikisma=0;
	}
	else if (durum_switch_kapali())
	{
		sayac_motorAdim=KAPANMA_ADIM_SAYISI;
		ledPeriyot=LED_NORMAL_KAPALI;
		motor1_sikisma=false;
		sayac_motor1_sikisma=0;

	}

	if (sayac_motorAdim < BASLANGIC_ADIMI)
	{
		sayac_motorAdim = BASLANGIC_ADIMI;
	} 
	else if (sayac_motorAdim > KAPANMA_ADIM_SAYISI)
	{
		sayac_motorAdim = KAPANMA_ADIM_SAYISI;
	}

}
/*
	Bu fonksiyon; 3 konumlu bir anahtar ile kap�y� "A��k","Kapal�" ve "Otomatik" 
	konumuna manuel olarak durumland�r�labilir.
*/
void anahtarKonumunaGoreSistemin_isletilmesi()
{
	switch (anahtarKonumu)
	{
		case 0:
		kapiyi_ac();
		break;
		
		case 1:
		calismaModlarininUygulanmasi();
		break;
		
		case 2:
		kapiyi_kapat();
		break;
	}
}
/*
	Bu fonksiyon; kap� kapan�rken araya s�k��may� �nlemek i�in 
	kap�n�n �n taraf�nda bulunan IR ���k perdesini aktif hale getirir,
	bir nesne tespit etti�inde istenen s�re kadar kap�y� a�ar bekler 
	ve tekrar kapat�r.
*/
void emniyetTedbirleri()
{

	if (!durum_optik_kapi_emniyeti())
	{
		calismaModu='E';
		ledAc();
		sayacEmniyet=0;
	}
	else if(motor1_sikisma)
	{
		if (sayac_motor1_sikisma < SIKISMADAN_KURTULMA_DENEME)
		{
			calismaModu='S';
		} 
		else
		{
			calismaModu='D';
// 			sayac_motorAdim=KAPANMA_ADIM_SAYISI;	//Motor durduruldu�unda kap� optik emniyetinin devre d��� kalmas� i�in
		}
	}
	else
	{
		if (sayacEmniyet>EMNIYET_BEKLEME_SURESI)
		{
			sayacEmniyet=EMNIYET_BEKLEME_SURESI+1;
			calismaModu='O';
		}
		ledKapat();
	}
}
/*
	Bu fonksiyon; farkl� durumlara g�re belirlenen �al��ma modunu uygular.
*/
void calismaModlarininUygulanmasi()
{
	emniyetTedbirleri();
			
	switch(calismaModu)
	{
		case 'D':
			motor1_dur();
			optik_kapi_emniyeti_disable();
			optik_Encoder_disable();
			
		break;
				
		case 'E':
			if (!durum_switch_kapali())	//Kap� kapal� iken tekrar a��lmamas� i�in switch durumuna ba�l� olarak kap� a�t�r�ld�
			{
				kapiyi_ac();
			}
			else
			{
				sayacEmniyet=EMNIYET_BEKLEME_SURESI+1;	//Emniyet sayac�ndan dolay� beklemeyi �nlemek i�in saya� g�venli de�ere ayarland�
			}
				
		break;
				
				
		case 'O':
				
			kapiyi_otomatik_acma_kapatma();
			if (!birSaniye)
			{
				kapi_acik_kapali_kontrolu();
			}
				
		break;
				
		case 'S':
			if (motor1_sikisma_yon)
			{
				kapiyi_ac();
			}
			else
			{
				kapiyi_kapat();
			}
	
		break;
		
		case 'T':
		
		break;
	}
}

/*
	Bu fonksiyon 1 saniye periyod ile �al���r. Fonksiyon i�inde ���k seviyesi �l��l�r, 
	anahtar konumu kontrol edilir, motor s�k���kl��� kontrol edilir.
*/
void zamanli_islemler()
{
	if (durum0)
	{
		if (birSaniye)
		{
			birSaniye=false;
			sayacEmniyet++;
			isikSeviyesi=(ADC_get_conversion(6));
			anahtarKonumu=(ADC_get_conversion(7)/340);//�� konumlu anahtar kullanaca��m�z i�in 340'a b�ld�m
			kapanmaGecikmesi=(ADC_get_conversion(4)*4);//1024*4/60=68 dakikal�k gecikme ayarlayabilmek i�in
			gece_gunduz_algilama();
			motor1_sikisiklik_kontrolu();
			uart_yazdir();
		}
	}
	else
	{
		birSaniye=true;
	}	
}

/*
	Bu fonksiyonda motorun d�zg�n �al���p �al��mad��� optik enkoder ile 
	kar��la�t�r�larak tespit edilir ve de�i�kene atan�r.
*/

void motor1_sikisiklik_kontrolu()
{
	if (motor1_enable())
	{
		if (sayac_motorAdim==onceki_sayac_motorAdim)
		{
			motor1_sikisma=true;
			motor1_sikisma_yon=motor1_yon;
			ledPeriyot=LED_ARIZA;
			sayac_motor1_sikisma++;
		}
		else
		{
			motor1_sikisma=false;
		}
	} 
	onceki_sayac_motorAdim=sayac_motorAdim;
}

void led_komuta()
{
	if (durumLed)
	{
		ledAc();
	}
	else
	{
		ledKapat();
	}
}

void uart_yazdir()
{
	char buffer[7];
	
	itoa( isikSeviyesi, buffer, 10);   // convert interger into string (decimal format)
	uart_puts("\t");
	uart_puts(buffer);        
	uart_puts("\t");

	itoa( sayacGece, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
	
	itoa( kapanmaGecikmesi, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");
	
	itoa( sayac_motorAdim, buffer, 10);
	uart_puts(buffer);
	uart_puts("\t");

	uart_putc(calismaModu);
	uart_puts("\n");

}
