plugins {
    id("com.android.library")
    id("org.jetbrains.kotlin.android")
    `maven-publish`
}

val libNamespace = "org.gs1.gs1androidlibrary"

afterEvaluate {
    publishing {
        publications {
            create<MavenPublication> (project.name) {
                from(components["release"])
                groupId = libNamespace
                artifactId = project.name
                version = "1.0.0"
            }
        }
    }
}

android {
    namespace = libNamespace
    compileSdk = 34

    defaultConfig {
        minSdk = 16

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        @Suppress("UnstableApiUsage")
        externalNativeBuild {
            @Suppress("UnstableApiUsage")
            cmake {
                cppFlags += ""
            }
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    afterEvaluate {
        android.libraryVariants.forEach { libraryVariant ->
            libraryVariant.outputs.forEach { _ ->
                val archivesBaseName = project.name
                val filePrefix = "$buildDir/outputs/aar/$archivesBaseName"
                val fileSuffix = "aar"
                val originalFile = file("$filePrefix-${libraryVariant.buildType.name}.$fileSuffix")
                val renamedFile = file("$filePrefix.$fileSuffix")
                tasks.named("assemble").configure {
                    doLast {
                        originalFile.renameTo(renamedFile)
                    }
                }
            }
        }
    }
}


dependencies {
    testImplementation("junit:junit:4.13.2")
}