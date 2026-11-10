import org.linkja.crypto.Library;

// Run against a library built with -DINCLUDE_SECRETS=OFF.
// createSecureHash needs the secret, so it must fail with a clear exception.
public class NoSecretsTest {
  public static void main(String[] args) {
    try {
      Library.createSecureHash("linkja", new byte[] { 0x01 }, "1001", "testToken1");
      System.out.println("**ERROR : createSecureHash returned in a no-secrets build");
    }
    catch (UnsupportedOperationException exc) {
      System.out.println("OK - createSecureHash throws: " + exc.getMessage());
      return;
    }
    catch (Throwable exc) {
      System.out.println("**ERROR : createSecureHash threw " + exc);
    }
    System.exit(1);
  }
}
