/*
ネイティブアプリ用のアイコンを順次書き出します。

iOS用アイコン
*512
*144
*72
*57

Android用アイコン
* 512
* 96
* 72
* 36
* 48

作成者：岩野
*/



#target photoshop

main();

function main(){
	outputPath = Folder.selectDialog("保存先ディレクトリを選択してください"),
	outputObj = new Folder(outputPath);
	if ( outputObj === null) {exit();} //キャンセル処理

	outputPrefix = prompt("ファイル名の接頭語を入力してください","");//入力ダイアログ
	
	var resolutions = [29, 40, 50, 57, 58, 72, 76, 80, 100, 114, 120, 144, 152];
	for( var i = 0; i <= resolutions.length; i++) {
		resolChange( resolutions[i] );//解像度を変更
		expFile( resolutions[i], outputObj,outputPrefix);//書き出す
		actionUndo();//アンドゥで元の解像度に戻す
	}
}

/*保存*/
function expFile( a, strPath ,strFile) {

	var idsave = charIDToTypeID( "save" );
		var desc11 = new ActionDescriptor();
		var idAs = charIDToTypeID( "As  " );
			var desc12 = new ActionDescriptor();
			var idEQlt = charIDToTypeID( "EQlt" );
			desc12.putInteger( idEQlt, 12 );
			var idMttC = charIDToTypeID( "MttC" );
			var idMttC = charIDToTypeID( "MttC" );
			var idNone = charIDToTypeID( "None" );
			desc12.putEnumerated( idMttC, idMttC, idNone );
		var idJPEG = charIDToTypeID( "JPEG" );
		desc11.putObject( idAs, idJPEG, desc12 );
  	var idIn = charIDToTypeID( "In  " );
		desc11.putPath( idIn, new File(strPath+"/"+strFile+"-"+a+".jpg") );
		var idCpy = charIDToTypeID( "Cpy " );
		desc11.putBoolean( idCpy, true );
		var idLwCs = charIDToTypeID( "LwCs" );
		desc11.putBoolean( idLwCs, true );
		var idEmbP = charIDToTypeID( "EmbP" );
		desc11.putBoolean( idEmbP, false );
//		alert(i);
	executeAction( idsave, desc11, DialogModes.NO );
}


/*アンドゥ*/
function actionUndo() {
	var docRef = app.activeDocument
	var savedLayer = docRef.activeLayer

	var idslct = charIDToTypeID( "slct" );
		var desc3 = new ActionDescriptor();
		var idnull = charIDToTypeID( "null" );
			var ref1 = new ActionReference();
			var idHstS = charIDToTypeID( "HstS" );
			var idOrdn = charIDToTypeID( "Ordn" );
			var idPrvs = charIDToTypeID( "Prvs" );
			ref1.putEnumerated( idHstS, idOrdn, idPrvs );
		desc3.putReference( idnull, ref1 );
	try
	{
		executeAction( idslct, desc3, DialogModes.NO );
	}
	catch(err)
	{}
	
	try
	{
		docRef.activeLayer = savedLayer
	}
	catch(err)
	{}
}


/*解像度変更*/
function resolChange(resol){
	var idImgS = charIDToTypeID( "ImgS" );
		var desc8 = new ActionDescriptor();
		var idWdth = charIDToTypeID( "Wdth" );
		var idPxl = charIDToTypeID( "#Pxl" );
		desc8.putUnitDouble( idWdth, idPxl, resol * 1.0 ); // 何かfloatを強いられてる感じだったので1.0倍しとく
		var idscaleStyles = stringIDToTypeID( "scaleStyles" );
		desc8.putBoolean( idscaleStyles, true );
		var idCnsP = charIDToTypeID( "CnsP" );
		desc8.putBoolean( idCnsP, true );
		var idIntr = charIDToTypeID( "Intr" );
		var idIntp = charIDToTypeID( "Intp" );
		var idBcbc = charIDToTypeID( "Bcbc" );
		desc8.putEnumerated( idIntr, idIntp, idBcbc );
	executeAction( idImgS, desc8, DialogModes.NO );
}

