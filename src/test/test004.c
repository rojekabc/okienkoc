#include <okienkoc/fileobject.h>
#include <stdio.h>

#define fail(arg) fprintf(stdout, arg)


int main()
{
	GOC_FILEOBJECT *fob = NULL;
	fileObjectInit();
	fob = fileObjectCreate();
	if ( fob == NULL )
		fail("FileObject not created");

	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), NULL ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectSetAttribute( fob, "alfa", "alfa");
	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa" ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectSetAttribute( fob, "beta", "beta");
	fileObjectSetAttribute( fob, "gamma", allocObject("Integer", 12, 0).result );
	fileObjectSerialize( fob, stdout );
	writeObjectToFile( "plik.txt", fob );

	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), "beta" ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa" ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectSetAttribute( fob, "beta", "Beta");
	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), "Beta" ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa" ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectRemoveAttribute( fob, "beta");
	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), "alfa" ) )
		fail("FileObject attribute alfa not correctly setted");

	fileObjectRemoveAttribute( fob, "alfa");
	fileObjectSerialize( fob, stdout );
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "beta"), NULL ) )
		fail("FileObject attribute beta not correctly setted");
	if ( !goc_stringEquals( fileObjectGetAttribute( fob, "alfa"), NULL ) )
		fail("FileObject attribute alfa not correctly setted");

	fob = fileObjectDestroy(fob);
	if ( fob != NULL )
		fail("FileObject not removed");
	return 0;
}
