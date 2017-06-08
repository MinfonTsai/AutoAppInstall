
#include "stdafx.h"
#include "Markup.h"


/*
 1. 檢查系統中 有無安裝指定的 APK -> "快圖瀏覽" ?
 2. 如果 No 無: 立即安裝APK
 3. 如果 Yes有: 修改preferred item 成我們要啟動的APK

*/
 
//findx = Find_packages_inXML( "com.alensw.PicFolder" );

bool Find_packages_inXML( char *package_name )
{

	CMarkup xml1;
	bool flag,flag1;
    char filename[500], *temp; 
	char att0[500],value0[500];
    char *found1;
    bool intent_find1;
 
	intent_find1 = false;
    flag1 = false;

	flag = xml1.Load(MCD_T("packages.xml"));
	if( !flag ) return false;
   
	xml1.FindElem();
	xml1.IntoElem();

	while( xml1.FindElem(MCD_T("package")) )
	{
		  std::string str_att5 = xml1.GetAttribName(0); // 可以找到 "name"
		  std::string str_value5 = xml1.GetAttrib(str_att5); 
		  strcpy( att0, str_value5.c_str() ); 
            if( !strcmp( att0 , package_name ) )
			{
				intent_find1 =true;
				break;
			}
	}

    if( intent_find1 )
	   return true;
	
 return false;

return false;
} 

void Fixed_preferred_Item(void)
{
	// TODO: Add your control notification handler code here
	//OnOK();
		CMarkup xml1;
	bool flag,flag1,flag2;
    char filename[500], *temp; 
	char att0[500],value0[500];
    char *found1;
    int  intent_find1,intent_find2;
   // string str_value0;
 
	std::string str_att0,str_att1,str_att2;
    std::string str_value0,str_value1,str_value2;

	intent_find1 = 0;
	intent_find2 = 0;

	flag = xml1.Load(MCD_T("packages.xml"));
	if( !flag ) return ;
   
	xml1.FindElem();
	xml1.IntoElem();

	flag = xml1.FindElem(MCD_T("preferred-activities"));
	//xml1.FindElem();
	xml1.IntoElem();

  //  flag =FindChildElem( MCD_T("filter") );
    //flag1 = xml1.FindElem(MCD_T("item"));
	//if( flag1 )

	while( xml1.FindElem( MCD_T("item") ) )
	{
		xml1.SavePos( MCD_T("item_in_preferred"),  0 );

		xml1.IntoElem();
		flag = xml1.FindElem(MCD_T("filter"));
		if( !flag ) break;

		xml1.IntoElem();
		flag = xml1.FindElem(MCD_T("action"));
		if( !flag ) break;
  
	

	str_att0 = xml1.GetAttribName(0);
	str_value0 = xml1.GetAttrib(str_att0);
 //	std::string strTitle = xml1.GetElemContent();

	strcpy( att0, str_att0.c_str() );    // string 和 LPTSTR 的轉換
    strcpy( value0, str_value0.c_str() ); 

	 if( strstr( value0 , "android.intent.action.MAIN" ) )
	 {
		  xml1.RestorePos(  MCD_T("item_in_preferred"),  0  );
			intent_find1 = 1;
	       // xml1.OutOfElem(); // out of filter
		   //	xml1.ResetMainPos();
		  //	flag = xml1.FindElem(MCD_T("set") );
            str_att1 = xml1.GetAttribName(0); // 可以找到 "name"
			str_value1 = xml1.GetAttrib(str_att1); 
			// 可以找到"info.tikusoft.launcher7/.MainScreen"
			strcpy( att0, str_att1.c_str() ); 
            if( !strcmp( att0 , "name" ) )
			xml1.SetAttrib( str_att1 , MCD_T("info.tikusoft.launcher7/.MainScreen"), 0);//修改成我們要啟動的APK

			//xml1.IntoElem();
		
	 }
	 else if( strstr( value0 , "android.intent.action.VIEW" ) )
	 {
		 xml1.RestorePos(  MCD_T("item_in_preferred"),  0  );

			intent_find2 = 1;

	    	// xml1.OutOfElem(); // out of filter
		    // xml1.ResetMainPos();
			//xml1.OutOfElem(); // out of item
			
            str_att2 = xml1.GetAttribName(0); // 可以找到 "name"
			str_value2 = xml1.GetAttrib(str_att2); 
			// 可以找到 "com.alensw.PicFolder/.PictureActivity"
			// 或可以找到 "com.cooliris.media/.Gallery"
			strcpy( att0, str_att2.c_str() ); 
            if( !strcmp( att0 , "name" ) )
			xml1.SetAttrib( str_att2 , MCD_T("com.alensw.PicFolder/.PictureActivity"), 0);//修改成我們要啟動的APK
			
           // break;
	 }

	}
	
	if( intent_find2 != 1 )
	{
		xml1.RestorePos(  MCD_T("item_in_preferred"),  0  );
		//xml1.OutOfElem(); 
		
		xml1.AddElem( "item","", 0 );
		xml1.AddAttrib( "name", "com.alensw.PicFolder/.PictureActivity" ); 
        xml1.AddAttrib( "match", "600000" ); 
		xml1.AddAttrib( "set", "1" ); 
		xml1.IntoElem();
		
		xml1.AddElem( "set","", 0 );
        xml1.AddAttrib( "name", "com.alensw.PicFolder/.PictureActivity" ); 
     
		xml1.AddElem( "filter","", 0 );
	
	    xml1.IntoElem(); 
    
		xml1.AddElem( "action","", 0 );
        xml1.AddAttrib( "name", "android.intent.action.VIEW" ); 
		xml1.AddElem( "cat","", 0 );
        xml1.AddAttrib( "name", "android.intent.category.DEFAULT" );
		xml1.AddElem( "type","", 0 );
        xml1.AddAttrib( "name", "image/*" );
   	}
  

   xml1.Save(MCD_T( "packages.xml" ));
}