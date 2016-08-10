package com.itsec.jniapitester;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import java.io.IOException;

class TestObject {
    private static final String TAG = "JNITesterObject";

	public int myField;
	private long secretField;
	public short shortField;
	public char charField;
	public byte byteField;
	public float floatField;
	public double doubleField;
	public TestObject inception;

	public TestObject() {
		myField = 43;
		secretField = 442;
		shortField = 5;
		charField = 'p';
		byteField = (byte)222;
		floatField = 7.45f;
		doubleField = 987.56;
		inception = this;
	}

	public TestObject(int myField, char charField, TestObject inception) {
		this.myField = myField;
		this.charField = charField;
		this.inception = inception;
	}

	public TestObject(int myField) {
		this.myField = myField;
		secretField = 442;
	}

	public void callSomething(int i) {
		Log.d(TAG, "something called with "+i);
	}

	public byte callSomethingByte() {
		Log.d(TAG, "callSomethingByte()");
		return (byte) myField;
	}

	public boolean callSomethingBoolean(int i, double d) {
		Log.d(TAG, "callSomethingBoolean(i="+i+", d="+d+")");
		return d > (double)i;
	}

	public int callSomethingInt() {
		Log.d(TAG, "callSomethingInt()");
		return 2*myField;
	}

	public Object callSomethingObject() {
		Log.d(TAG, "CallSomethingObject()");
		return this;
	}

	public static void printInt(int i) {
		Log.d(TAG, "static printInt: this is the magic number: "+i);
	}

	public static short staticNum(short s) {
		Log.d(TAG, "static short method with param "+s);
		return (short)(s+1);
	}
}

class SubObject extends TestObject {

}

public class JniApiTester extends Activity implements OnClickListener {
    private static final String TAG = "JNIApiTester";
	private static final String testString = "someString";

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
	setContentView(R.layout.layout);

	Button startTest = (Button) findViewById(R.id.btn_starttest);
	startTest.setOnClickListener(this);
    }

    public void onClick(View v) {
		if (v.getId() == R.id.btn_starttest) {
			EditText txtRepetitions = (EditText) findViewById(R.id.input_repetitions);
	    	String n = txtRepetitions.getText().toString();
			int n2 = Integer.parseInt(n);
			Log.d(TAG, "Running test "+n2+" times");
			runTest(n2);
		}
    }

	private void runTest(int n) {
		for (int i=0; i<n; i++) {
			int success = 0;
			Log.d(TAG, "test run #"+(i+1));

			if (testGetStringUTFChars(testString)) success++;

			String s = testNewStringUTF();
			if (s.equals(testString)) {
				Log.d(TAG, "NewStringUTF: SUCCESS");
				success++;
			} else {
				Log.d(TAG, "NewStringUTF: FAIL ("+s+" != "+testString);
			}

			if(testGetVersion()) success++;

			Class c = testFindClass();
			if (c.getName().equals("android.app.Activity")) {
				Log.d(TAG, "FindClass: SUCCESS");
				success++;
			} else
				Log.d(TAG, "FindClass: FAIL android.app.Activity!="+c.getName());

			Class c2 = testGetSuperclass(c);
			if (c2.getName().equals("android.view.ContextThemeWrapper")) {
				Log.d(TAG, "GetSuperclass(): SUCCESS");
				success++;
			} else
				Log.d(TAG, "GetSuperclass(): FAIL expected android.view.ContextThemeWrapper but got "
					+c2.getName());

			boolean b = testIsAssignableFrom(c2,c);
			Log.d(TAG, "IsAssignableFrom(): "+c2.getName()+"->"+c.getName()+": "+b);
			Log.d(TAG, "IsAssignableFrom(): "+c.getName()+"->"+c2.getName()+": "+testIsAssignableFrom(c,c2));
			if (!b) {
				success++;
				Log.d(TAG, "IsAssignableFrom(): SUCCESS");
			}

			IOException ioe = new IOException();
			Class ioec = ioe.getClass();
			Object ioeo = testAllocObject(ioec);
			if (ioeo != null) {
				success++;
				Log.d(TAG, "AllocObject(): SUCCESS");
			} else Log.d(TAG, "AllocObjct(): FAIL");

			if (ioeo != testAllocObject(ioec)) {
				success++;
				Log.d(TAG, "IsSameObject(): SUCCESS");
			} else Log.d(TAG, "IsSameObject(): FAIL");

			int thrown = 0;
			try {
				thrown = testThrow((IOException)ioeo);
				Log.d(TAG, "Throw(): FAIL Exception doesn't fly");
			} catch (IOException e) {
				Log.d(TAG, "Threw myself a "+thrown);
				if (thrown == 0) {
					success++;
					Log.d(TAG, "Throw(): SUCCESS");
				} else Log.d(TAG, "Throw(): FAIL 0!="+thrown);
			}

			try {
				thrown = testThrowNew(ioec);
				Log.d(TAG, "ThrowNew(): FAIL Exception doesn't fly");
			} catch (IOException e) {
				if (e.getMessage().equals("Throw it high and wide")) {
					success++;
					Log.d(TAG, "ThrowNew(): SUCCESS");
				} else Log.d(TAG, "ThrowNew(): FAIL unexpected message: "+e.getMessage());
			}

			Throwable t = testExceptionOccurred(ioec);
			if (t.getMessage().equals("cheeky exception")) {
				success++;
				Log.d(TAG, "ExceptionOccurred(): SUCCESS");
			} else Log.d(TAG, "ExceptionOccurred(): FAIL unexpected message: "+t.getMessage());

			boolean exceptionCheck = false;
			try {
				exceptionCheck = testExceptionCheck(ioec);
			} catch (IOException e) {
				if (exceptionCheck) {
					success++;
					Log.d(TAG, "ExceptionCheck(): SUCCESS");
				} else Log.d(TAG, "ExceptionCheck(): FAIL, exception not found");
			}

			TestObject to = new TestObject();
			testGetMethodID(to.getClass());
			testGetStaticMethodID(to.getClass());
			short sho = testCallStaticShortMethod(to.getClass());
			if (sho == 499) {
				success++;
				Log.d(TAG, "CallStaticShortMethod(): SUCCESS");
			} else Log.d(TAG, "CallStaticShortMethod(): FAIL");

			if (testCallSomethingBoolean(to, to.getClass(), 5, 7.5)) {
				success++;
				Log.d(TAG, "CallBooleanMethodV(): SUCCESS");
			} else Log.d(TAG, "CallBooleanMethodV(): FAIL");

			SubObject subTo = new SubObject();
			if (testCallNonvirtual(subTo, to.getClass()))
				success++;

			success += testGetFieldID(to);

			try {
				success += testFromReflectedField(to, to.getClass().getDeclaredField("myField"));
			} catch (NoSuchFieldException e) {
				Log.e(TAG, e.getMessage());
			}

			success += testGetObjectField(to, to.inception);

			success += testGetCharField(to, to.charField);

			if (testIsInstanceOf(to, to.getClass())) {
				success++;
				Log.d(TAG, "IsInstanceOf(): SUCCESS");
			} else Log.d(TAG, "IsInstanceOf(): FAIL TestObject not instance of TestObject class");

			short[] sArr = {2,3,5,7,11,13,17,19};
			byte[] bArr = {2,3,5,7,11,13,17,19};
			if (testSetShortArrayRegion(sArr)) {
				success++;
				Log.d(TAG, "SetShortArrayRegion(): SUCCESS");
			} else Log.d(TAG, "SetShortArrayRegion(): FAIL");
			Log.d(TAG, "array={"+sArr[0]+","+sArr[1]+","+sArr[2]+","
				+sArr[3]+","+sArr[4]+","+sArr[5]+","+sArr[6]+","+sArr[7]+"}");
			if (testGetByteArrayRegion(bArr)) {
				success++;
				Log.d(TAG, "GetByteArrayRegion(): SUCCESS");
			} else Log.d(TAG, "GetByteArrayRegion(): FAIL");
			
			String someString = "get these chars";
			testGetStringChars(someString);
			Log.d(TAG, "someString="+someString);

			if (testNewObject(to.getClass())) success++;

			testArrayCritical(sArr);
			Log.d(TAG, "array={"+sArr[0]+","+sArr[1]+","+sArr[2]+","
				+sArr[3]+","+sArr[4]+","+sArr[5]+","+sArr[6]+","+sArr[7]+"}");

			Log.d(TAG, "successful tests: "+success);
		}
	}

	private native boolean testGetStringUTFChars(String s);
	private native String testNewStringUTF();
	private native boolean testGetVersion();
	private native Class testFindClass();
	private native Class testGetSuperclass(Class c);
	private native boolean testIsAssignableFrom(Class c1, Class c2);
	private native Object testAllocObject(Class c);
	private native int testThrow(Throwable t) throws IOException;
	private native int testThrowNew(Class c) throws IOException;
	private native Throwable testExceptionOccurred(Class c);
	private native boolean testExceptionCheck(Class c) throws IOException;
	private native void testGetMethodID(Class c);
	private native void testGetStaticMethodID(Class c);
	private native short testCallStaticShortMethod(Class c);
	private native int testGetFieldID(Object o);
	private native int testFromReflectedField(Object obj, Object jfield);
	private native void testCallSomethingByte(Class c);
	private native boolean testCallSomethingBoolean(Object obj, Class c, int i, double d);
	private native boolean testCallNonvirtual(Object obj, Class superclass);
	private native boolean testIsInstanceOf(Object o, Class c);
	private native boolean testIsSameObject(Object o, Class c);
	private native int testGetObjectField(Object o, Object result);
	private native int testGetCharField(Object o, char result);
	private native boolean testSetShortArrayRegion(short[] a);
	private native boolean testGetByteArrayRegion(byte[] a);
	private native void testGetStringChars(String s);
	private native boolean testNewObject(Class c);
	private native void testArrayCritical(short[] a);

    static {
        System.loadLibrary("jniApiTester");
    }
}
