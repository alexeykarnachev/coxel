const int INDS256[256] = int[256](151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180);


const float VALS256[256] = float[256]( 0.0901379979912359, 0.08866898508738652, 0.1802483469293641, 0.8183978725291774, 0.6455131421937433, 0.4726400783492488, 0.5082446687992332, 0.27234383533135553, 0.4019207774960265, 0.7786119660911042, 0.3211543002134776, 0.1437743485573515, 0.591062319212196, 0.2578537486072795, 0.7568283740694545, 0.2799313833304935, 0.5055633973221071, 0.8353614031530537, 0.09787514016839327, 0.7571492394863675, 0.6933020299533245, 0.27412271414921274, 0.036220193970115444, 0.9550438362892577, 0.9811549179672195, 0.18189273965017283, 0.695051247040141, 0.8469417174382918, 0.6151202898890461, 0.24112082865466777, 0.8167744946396718, 0.9986365829235, 0.18512095698577724, 0.9511164743608932, 0.7124210348220267, 0.6134784305801669, 0.9136888962819694, 0.9412688341154785, 0.3132691181372077, 0.4846859132477769, 0.44805879666313553, 0.08861588557450628, 0.37568645278043544, 0.5255591834338195, 0.8146082284563668, 0.2745159834665959, 0.9348472389048057, 0.31793497507960966, 0.40540068920539285, 0.882553509691131, 0.5572619589008615, 0.040052966994296835, 0.011899837607582064, 0.5024001587207407, 0.7009632148900543, 0.2711721637317309, 0.9449371541858322, 0.24447204305375503, 0.4247371614748072, 0.935664436128515, 0.8683330276265334, 0.22456029473401606, 0.07963281477851591, 0.9130530391911029, 0.8433010201273612, 0.4011077279116606, 0.34184598657057996, 0.23324981357856744, 0.6383914821872787, 0.12120953605156715, 0.4533909544205026, 0.04366611619090477, 0.9874706811651551, 0.6077542032432428, 0.6699333541407884, 0.9545388522916152, 0.42063102741957414, 0.7931454967613109, 0.43833466406594246, 0.19226383269247382, 0.40876075706141324, 0.476711910951624, 0.8107838938139252, 0.4880350253206621, 0.4367367456130924, 0.025141777236656337, 0.12112766192491531, 0.5907604221809475, 0.7978238067944486, 0.25898696228888207, 0.7504705444296609, 0.4144501580149036, 0.8548992896533528, 0.08620211110108744, 0.66730773780664, 0.3222306837630097, 0.8044107512637703, 0.6335315679313798, 0.49052077530303406, 0.4074417082314665, 0.986437028280056, 0.8879109111503098, 0.7801964797599973, 0.894228797671969, 0.5889332899872747, 0.32668216611441403, 0.46053551236775037, 0.8548383405869917, 0.6891831316053936, 0.046589325304294604, 0.056961911553429334, 0.36344979299997027, 0.9855625735992315, 0.9311717298604087, 0.5112193541349968, 0.728758055570425, 0.16414186593689617, 0.46289938754414517, 0.10298063603356222, 0.5316378273346686, 0.7096678892493894, 0.07267087206586564, 0.7569229016108315, 0.8570244702139163, 0.26805505565040577, 0.07994942835428387, 0.6225323175874022, 0.9414850672644135, 0.38557071420399525, 0.7648001312197229, 0.8144906451298318, 0.13522471840912087, 0.10978048271914254, 0.20632468773337787, 0.08557962340206249, 0.9361478527766681, 0.9290316776520171, 0.99420546796571, 0.7201784356385011, 0.48050001251066365, 0.4209926950288212, 0.7497691080636605, 0.7538346667228276, 0.21664782704113272, 0.19223340749571516, 0.14307999995747533, 0.0953098811401687, 0.2294435603691446, 0.256859225179754, 0.5304387826899225, 0.8385881458524073, 0.10868277442495211, 0.295588814481325, 0.2563208677046207, 0.20965114593000422, 0.10142781749338703, 0.44004788387066796, 0.8974098607067253, 0.6933503772372365, 0.8473733613314568, 0.6637506254874945, 0.918247287995333, 0.035877676114570156, 0.5570693838235161, 0.5368929728082943, 0.40711323385415554, 0.8835412557942087, 0.26602930030912775, 0.18501283504891508, 0.37583199793247046, 0.28856452696867285, 0.10607489058939135, 0.34277049964134776, 0.776819923586848, 0.44700452454770234, 0.49712020598382756, 0.9393397475428459, 0.6007392208747174, 0.6702635327763607, 0.24360513374402926, 0.38897328382865115, 0.09355454533465102, 0.8998426324151525, 0.5412003823803327, 0.343397511078562, 0.9791006597389902, 0.3929676556745143, 0.3598678890357212, 0.44427594840370455, 0.8150794134613553, 0.9043260910295017, 0.4741628937608121, 0.7445843763724916, 0.2068475369126992, 0.1496939099727812, 0.1496149064661797, 0.8174817618244039, 0.2903409488088249, 0.23441484272397473, 0.8265210701273589, 0.6867399357385315, 0.2722489271542624, 0.65232389307785, 0.6685858664929446, 0.04250291455282662, 0.5387754953898314, 0.29690508313837904, 0.9376127036898982, 0.3692920006702711, 0.6152527572990696, 0.614323169126445, 0.08621666410955153, 0.4716928946792427, 0.9553500901762164, 0.6354011022908094, 0.6111236232621502, 0.09813286053259518, 0.9128194352380188, 0.34079541223459564, 0.758587022910277, 0.14849332465905984, 0.22283170373119254, 0.7445375631877007, 0.38511713126731173, 0.7793339355521065, 0.5754940894443084, 0.000732686431349272, 0.9890571935172477, 0.6064693071494773, 0.6888978351185353, 0.24198877795472284, 0.8784388226000001, 0.5470883449942411, 0.11721824707334827, 0.5700421991628873, 0.261870377622496, 0.08685897076898197, 0.06949523528785018, 0.11711312387690798, 0.4634802676937362, 0.9600494601584412, 0.8295478281567205, 0.12789063361306874, 0.746399993277336, 0.9953435905481874, 0.05502333359339018, 0.41799916389631586, 0.36879566803579567, 0.9457672270463515, 0.3574319660894867, 0.5185678003875118, 0.8113103923554358, 0.9829980150731118, 0.6835621453782552, 0.49538973457404, 0.6479045126214759 );

const vec2 POISSON_DISK64[64] = vec2[64](
    vec2(-0.613392, 0.617481),
    vec2(0.170019, -0.040254),
    vec2(-0.299417, 0.791925),
    vec2(0.645680, 0.493210),
    vec2(-0.651784, 0.717887),
    vec2(0.421003, 0.027070),
    vec2(-0.817194, -0.271096),
    vec2(-0.705374, -0.668203),
    vec2(0.977050, -0.108615),
    vec2(0.063326, 0.142369),
    vec2(0.203528, 0.214331),
    vec2(-0.667531, 0.326090),
    vec2(-0.098422, -0.295755),
    vec2(-0.885922, 0.215369),
    vec2(0.566637, 0.605213),
    vec2(0.039766, -0.396100),
    vec2(0.751946, 0.453352),
    vec2(0.078707, -0.715323),
    vec2(-0.075838, -0.529344),
    vec2(0.724479, -0.580798),
    vec2(0.222999, -0.215125),
    vec2(-0.467574, -0.405438),
    vec2(-0.248268, -0.814753),
    vec2(0.354411, -0.887570),
    vec2(0.175817, 0.382366),
    vec2(0.487472, -0.063082),
    vec2(-0.084078, 0.898312),
    vec2(0.488876, -0.783441),
    vec2(0.470016, 0.217933),
    vec2(-0.696890, -0.549791),
    vec2(-0.149693, 0.605762),
    vec2(0.034211, 0.979980),
    vec2(0.503098, -0.308878),
    vec2(-0.016205, -0.872921),
    vec2(0.385784, -0.393902),
    vec2(-0.146886, -0.859249),
    vec2(0.643361, 0.164098),
    vec2(0.634388, -0.049471),
    vec2(-0.688894, 0.007843),
    vec2(0.464034, -0.188818),
    vec2(-0.440840, 0.137486),
    vec2(0.364483, 0.511704),
    vec2(0.034028, 0.325968),
    vec2(0.099094, -0.308023),
    vec2(0.693960, -0.366253),
    vec2(0.678884, -0.204688),
    vec2(0.001801, 0.780328),
    vec2(0.145177, -0.898984),
    vec2(0.062655, -0.611866),
    vec2(0.315226, -0.604297),
    vec2(-0.780145, 0.486251),
    vec2(-0.371868, 0.882138),
    vec2(0.200476, 0.494430),
    vec2(-0.494552, -0.711051),
    vec2(0.612476, 0.705252),
    vec2(-0.578845, -0.768792),
    vec2(-0.772454, -0.090976),
    vec2(0.504440, 0.372295),
    vec2(0.155736, 0.065157),
    vec2(0.391522, 0.849605),
    vec2(-0.620106, -0.328104),
    vec2(0.789239, -0.419965),
    vec2(-0.545396, 0.538133),
    vec2(-0.178564, -0.596057)
);

#define ONE_OVER_SQRT2 0.70710678118

const vec2 GRADS2D[8] = vec2[8](
    vec2(0.0, 1.0),
    vec2(ONE_OVER_SQRT2, ONE_OVER_SQRT2),
    vec2(1.0, 0.0),
    vec2(ONE_OVER_SQRT2, -ONE_OVER_SQRT2),
    vec2(0.0, -1.0),
    vec2(-ONE_OVER_SQRT2, -ONE_OVER_SQRT2),
    vec2(-1.0, 0.0),
    vec2(-ONE_OVER_SQRT2, ONE_OVER_SQRT2)
);

const vec3 GRADS3D[12] = vec3[12](
    vec3(1.0, 1.0, 0.0),
    vec3(-1.0, 1.0, 0.0),
    vec3(1.0, -1.0, 0.0),
    vec3(-1.0, -1.0, 0.0),
    vec3(1.0, 0.0, 1.0),
    vec3(-1.0, 0.0, 1.0),
    vec3(1.0, 0.0, -1.0),
    vec3(-1.0, 0.0, -1.0),
    vec3(0.0, 1.0, 1.0),
    vec3(0.0, -1.0, 1.0),
    vec3(0.0, 1.0, -1.0),
    vec3(0.0, -1.0, -1.0)
);

const vec4 GRADS4D[32] = vec4[32](
    vec4(0,1,1,1),
    vec4(0,1,1,-1),
    vec4(0,1,-1,1),
    vec4(0,1,-1,-1),
    vec4(0,-1,1,1),
    vec4(0,-1,1,-1),
    vec4(0,-1,-1,1),
    vec4(0,-1,-1,-1),
    vec4(1,0,1,1),
    vec4(1,0,1,-1),
    vec4(1,0,-1,1),
    vec4(1,0,-1,-1),
    vec4(-1,0,1,1),
    vec4(-1,0,1,-1),
    vec4(-1,0,-1,1),
    vec4(-1,0,-1,-1),
    vec4(1,1,0,1),
    vec4(1,1,0,-1),
    vec4(1,-1,0,1),
    vec4(1,-1,0,-1),
    vec4(-1,1,0,1),
    vec4(-1,1,0,-1),
    vec4(-1,-1,0,1),
    vec4(-1,-1,0,-1),
    vec4(1,1,1,0),
    vec4(1,1,-1,0),
    vec4(1,-1,1,0),
    vec4(1,-1,-1,0),
    vec4(-1,1,1,0),
    vec4(-1,1,-1,0),
    vec4(-1,-1,1,0),
    vec4(-1,-1,-1,0)
);


float quintic_step(float x) {
    return x * x * x * (x * (x * 6. - 15.) + 10.);
}

float noise(float x) {
    int c1 = int(floor(x)) & 255;
    int c2 = (c1 + 1) & 255;

    float y1 = VALS256[c1];
    float y2 = VALS256[c2];

    float t = quintic_step(fract(x));
    float y = mix(y1, y2, t);
     
    return y;
}

float noise(float x, float y) {
    int cx1 = int(floor(x)) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(floor(y)) & 255;
    int cy2 = (cy1 + 1) & 255;

    float z1 = VALS256[INDS256[(INDS256[cx1] + cy1) & 255]];
    float z2 = VALS256[INDS256[(INDS256[cx2] + cy1) & 255]];
    float z3 = VALS256[INDS256[(INDS256[cx1] + cy2) & 255]];
    float z4 = VALS256[INDS256[(INDS256[cx2] + cy2) & 255]];

    float tx = quintic_step(fract(x));
    float ty = quintic_step(fract(y));

    float zx1 = mix(z1, z2, tx);
    float zx2 = mix(z3, z4, tx);
    float z = mix(zx1, zx2, ty);

    return z;
}

float noise(float x, float y, float z) {
    int cx1 = int(floor(x)) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(floor(y)) & 255;
    int cy2 = (cy1 + 1) & 255;
    int cz1 = int(floor(z)) & 255;
    int cz2 = (cz1 + 1) & 255;

    float w1 = VALS256[
        (INDS256[(INDS256[cx1] + cy1) & 255] + cz1) & 255];
    float w2 = VALS256[
        (INDS256[(INDS256[cx1] + cy1) & 255] + cz2) & 255];
    float w3 = VALS256[
        (INDS256[(INDS256[cx2] + cy1) & 255] + cz1) & 255];
    float w4 = VALS256[
        (INDS256[(INDS256[cx2] + cy1) & 255] + cz2) & 255];
    float w5 = VALS256[
        (INDS256[(INDS256[cx1] + cy2) & 255] + cz1) & 255];
    float w6 = VALS256[
        (INDS256[(INDS256[cx1] + cy2) & 255] + cz2) & 255];
    float w7 = VALS256[
        (INDS256[(INDS256[cx2] + cy2) & 255] + cz1) & 255];
    float w8 = VALS256[
        (INDS256[(INDS256[cx2] + cy2) & 255] + cz2) & 255];

    float tx = quintic_step(fract(x));
    float ty = quintic_step(fract(y));
    float tz = quintic_step(fract(z));

    float wx1 = mix(w1, w3, tx);
    float wx2 = mix(w2, w4, tx);
    float wx3 = mix(w5, w7, tx);
    float wx4 = mix(w6, w8, tx);

    float wy1 = mix(wx1, wx3, ty);
    float wy2 = mix(wx2, wx4, ty);

    float w = mix(wy1, wy2, tz);

    return w;
}

float noise(float x, float y, float z, float u) {
    int cx1 = int(floor(x)) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(floor(y)) & 255;
    int cy2 = (cy1 + 1) & 255;
    int cz1 = int(floor(z)) & 255;
    int cz2 = (cz1 + 1) & 255;
    int cu1 = int(floor(u)) & 255;
    int cu2 = (cu1 + 1) & 255;

    float u1 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy1) & 255] + cz1) & 255] + cu1) & 255];
    float u2 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy1) & 255] + cz1) & 255] + cu2) & 255];
    float u3 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy1) & 255] + cz2) & 255] + cu1) & 255];
    float u4 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy1) & 255] + cz2) & 255] + cu2) & 255];
    float u5 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy1) & 255] + cz1) & 255] + cu1) & 255];
    float u6 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy1) & 255] + cz1) & 255] + cu2) & 255];
    float u7 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy1) & 255] + cz2) & 255] + cu1) & 255];
    float u8 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy1) & 255] + cz2) & 255] + cu2) & 255];
    float u9 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy2) & 255] + cz1) & 255] + cu1) & 255];
    float u10 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy2) & 255] + cz1) & 255] + cu2) & 255];
    float u11 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy2) & 255] + cz2) & 255] + cu1) & 255];
    float u12 = VALS256[
        (INDS256[(INDS256[(INDS256[cx1] + cy2) & 255] + cz2) & 255] + cu2) & 255];
    float u13 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy2) & 255] + cz1) & 255] + cu1) & 255];
    float u14 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy2) & 255] + cz1) & 255] + cu2) & 255];
    float u15 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy2) & 255] + cz2) & 255] + cu1) & 255];
    float u16 = VALS256[
        (INDS256[(INDS256[(INDS256[cx2] + cy2) & 255] + cz2) & 255] + cu2) & 255];

    float tx = quintic_step(fract(x));
    float ty = quintic_step(fract(y));
    float tz = quintic_step(fract(z));
    float tu = quintic_step(fract(u));

    float wu1 = mix(u1, u2, tu);
    float wu2 = mix(u3, u4, tu);
    float wu3 = mix(u5, u6, tu);
    float wu4 = mix(u7, u8, tu);
    float wu5 = mix(u9, u10, tu);
    float wu6 = mix(u11, u12, tu);
    float wu7 = mix(u13, u14, tu);
    float wu8 = mix(u15, u16, tu);

    float wx1 = mix(wu1, wu3, tx);
    float wx2 = mix(wu2, wu4, tx);
    float wx3 = mix(wu5, wu7, tx);
    float wx4 = mix(wu6, wu8, tx);

    float wy1 = mix(wx1, wx3, ty);
    float wy2 = mix(wx2, wx4, ty);

    float w = mix(wy1, wy2, tz);

    return w;
}

float perlin_noise(float x, float y) {
    int cx1 = int(floor(x)) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(floor(y)) & 255;
    int cy2 = (cy1 + 1) & 255;

    vec2 g1 = GRADS2D[INDS256[(INDS256[cx1] + cy1) & 255] % 8];
    vec2 g2 = GRADS2D[INDS256[(INDS256[cx2] + cy1) & 255] % 8];
    vec2 g3 = GRADS2D[INDS256[(INDS256[cx1] + cy2) & 255] % 8];
    vec2 g4 = GRADS2D[INDS256[(INDS256[cx2] + cy2) & 255] % 8];

    float tx = fract(x);
    float ty = fract(y);

    vec2 t1 = vec2(tx , 1.0 - ty);
    vec2 t2 = vec2(tx - 1.0, 1.0 - ty);
    vec2 t3 = vec2(tx, ty);
    vec2 t4 = vec2(tx - 1.0, ty);

    float z1 = dot(g1, t1);
    float z2 = dot(g2, t2);
    float z3 = dot(g3, t3);
    float z4 = dot(g4, t4);

    tx = quintic_step(tx);
    ty = quintic_step(ty);

    float zx1 = mix(z1, z2, tx);
    float zx2 = mix(z3, z4, tx);
    float z = mix(zx1, zx2, ty);

    z = clamp(z, -1.0, 1.0);
    z = (z + 1.0) / 2.0;
    return z;
}

float perlin_noise(float x, float y, float z) {
    int cx1 = int(floor(x)) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(floor(y)) & 255;
    int cy2 = (cy1 + 1) & 255;
    int cz1 = int(floor(z)) & 255;
    int cz2 = (cz1 + 1) & 255;

    vec3 g1 = GRADS3D[INDS256[(cx1 + INDS256[(cy1 + INDS256[cz1]) & 255]) & 255] % 12];
    vec3 g2 = GRADS3D[INDS256[(cx2 + INDS256[(cy1 + INDS256[cz1]) & 255]) & 255] % 12];
    vec3 g3 = GRADS3D[INDS256[(cx1 + INDS256[(cy2 + INDS256[cz1]) & 255]) & 255] % 12];
    vec3 g4 = GRADS3D[INDS256[(cx2 + INDS256[(cy2 + INDS256[cz1]) & 255]) & 255] % 12];
    vec3 g5 = GRADS3D[INDS256[(cx1 + INDS256[(cy1 + INDS256[cz2]) & 255]) & 255] % 12];
    vec3 g6 = GRADS3D[INDS256[(cx2 + INDS256[(cy1 + INDS256[cz2]) & 255]) & 255] % 12];
    vec3 g7 = GRADS3D[INDS256[(cx1 + INDS256[(cy2 + INDS256[cz2]) & 255]) & 255] % 12];
    vec3 g8 = GRADS3D[INDS256[(cx2 + INDS256[(cy2 + INDS256[cz2]) & 255]) & 255] % 12];

    float tx = fract(x);
    float ty = fract(y);
    float tz = fract(z);

    vec3 t1 = vec3(tx, ty, tz);
    vec3 t2 = vec3(tx - 1.0, ty, tz);
    vec3 t3 = vec3(tx, ty - 1.0, tz);
    vec3 t4 = vec3(tx - 1.0, ty - 1.0, tz);
    vec3 t5 = vec3(tx, ty, tz - 1.0);
    vec3 t6 = vec3(tx - 1.0, ty, tz - 1.0);
    vec3 t7 = vec3(tx, ty - 1.0, tz - 1.0);
    vec3 t8 = vec3(tx - 1.0, ty - 1.0, tz - 1.0);

    float w1 = dot(g1, t1);
    float w2 = dot(g2, t2);
    float w3 = dot(g3, t3);
    float w4 = dot(g4, t4);
    float w5 = dot(g5, t5);
    float w6 = dot(g6, t6);
    float w7 = dot(g7, t7);
    float w8 = dot(g8, t8);

    tx = quintic_step(tx);
    ty = quintic_step(ty);
    tz = quintic_step(tz);

    float wx1 = mix(w1, w2, tx);
    float wx2 = mix(w3, w4, tx);
    float wx3 = mix(w5, w6, tx);
    float wx4 = mix(w7, w8, tx);

    float wy1 = mix(wx1, wx2, ty);
    float wy2 = mix(wx3, wx4, ty);

    float w = mix(wy1, wy2, tz);

    w = clamp(w, -1.0, 1.0);
    w = (w + 1.0) / 2.0;
    return w;
}

float perlin_noise(float x, float y, float z, float u) {
    int cx1 = int(floor(x)) & 255;
    int cx2 = (cx1 + 1) & 255;
    int cy1 = int(floor(y)) & 255;
    int cy2 = (cy1 + 1) & 255;
    int cz1 = int(floor(z)) & 255;
    int cz2 = (cz1 + 1) & 255;
    int cu1 = int(floor(u)) & 255;
    int cu2 = (cu1 + 1) & 255;

    vec4 g1 = GRADS4D[INDS256[(cx1 + INDS256[(cy1 + INDS256[(cz1 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g2 = GRADS4D[INDS256[(cx2 + INDS256[(cy1 + INDS256[(cz1 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g3 = GRADS4D[INDS256[(cx1 + INDS256[(cy2 + INDS256[(cz1 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g4 = GRADS4D[INDS256[(cx2 + INDS256[(cy2 + INDS256[(cz1 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g5 = GRADS4D[INDS256[(cx1 + INDS256[(cy1 + INDS256[(cz2 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g6 = GRADS4D[INDS256[(cx2 + INDS256[(cy1 + INDS256[(cz2 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g7 = GRADS4D[INDS256[(cx1 + INDS256[(cy2 + INDS256[(cz2 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g8 = GRADS4D[INDS256[(cx2 + INDS256[(cy2 + INDS256[(cz2 + INDS256[cu1]) & 255]) & 255]) & 255] % 32];
    vec4 g9 = GRADS4D[INDS256[(cx1 + INDS256[(cy1 + INDS256[(cz1 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];
    vec4 g10 = GRADS4D[INDS256[(cx2 + INDS256[(cy1 + INDS256[(cz1 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];
    vec4 g11 = GRADS4D[INDS256[(cx1 + INDS256[(cy2 + INDS256[(cz1 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];
    vec4 g12 = GRADS4D[INDS256[(cx2 + INDS256[(cy2 + INDS256[(cz1 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];
    vec4 g13 = GRADS4D[INDS256[(cx1 + INDS256[(cy1 + INDS256[(cz2 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];
    vec4 g14 = GRADS4D[INDS256[(cx2 + INDS256[(cy1 + INDS256[(cz2 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];
    vec4 g15 = GRADS4D[INDS256[(cx1 + INDS256[(cy2 + INDS256[(cz2 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];
    vec4 g16 = GRADS4D[INDS256[(cx2 + INDS256[(cy2 + INDS256[(cz2 + INDS256[cu2]) & 255]) & 255]) & 255] % 32];

    float tx = fract(x);
    float ty = fract(y);
    float tz = fract(z);
    float tu = fract(u);

    vec4 t1 = vec4(tx, ty, tz, tu);
    vec4 t2 = vec4(tx - 1.0, ty, tz, tu);
    vec4 t3 = vec4(tx, ty - 1.0, tz, tu);
    vec4 t4 = vec4(tx - 1.0, ty - 1.0, tz, tu);
    vec4 t5 = vec4(tx, ty, tz - 1.0, tu);
    vec4 t6 = vec4(tx - 1.0, ty, tz - 1.0, tu);
    vec4 t7 = vec4(tx, ty - 1.0, tz - 1.0, tu);
    vec4 t8 = vec4(tx - 1.0, ty - 1.0, tz - 1.0, tu);
    vec4 t9 = vec4(tx, ty, tz, tu - 1.0);
    vec4 t10 = vec4(tx - 1.0, ty, tz, tu - 1.0);
    vec4 t11 = vec4(tx, ty - 1.0, tz, tu - 1.0);
    vec4 t12 = vec4(tx - 1.0, ty - 1.0, tz, tu - 1.0);
    vec4 t13 = vec4(tx, ty, tz - 1.0, tu - 1.0);
    vec4 t14 = vec4(tx - 1.0, ty, tz - 1.0, tu - 1.0);
    vec4 t15 = vec4(tx, ty - 1.0, tz - 1.0, tu - 1.0);
    vec4 t16 = vec4(tx - 1.0, ty - 1.0, tz - 1.0, tu - 1.0);

    float w1 = dot(g1, t1);
    float w2 = dot(g2, t2);
    float w3 = dot(g3, t3);
    float w4 = dot(g4, t4);
    float w5 = dot(g5, t5);
    float w6 = dot(g6, t6);
    float w7 = dot(g7, t7);
    float w8 = dot(g8, t8);
    float w9 = dot(g9, t9);
    float w10 = dot(g10, t10);
    float w11 = dot(g11, t11);
    float w12 = dot(g12, t12);
    float w13 = dot(g13, t13);
    float w14 = dot(g14, t14);
    float w15 = dot(g15, t15);
    float w16 = dot(g16, t16);

    tx = quintic_step(tx);
    ty = quintic_step(ty);
    tz = quintic_step(tz);
    tu = quintic_step(tu);

    float wx1 = mix(w1, w2, tx);
    float wx2 = mix(w3, w4, tx);
    float wx3 = mix(w5, w6, tx);
    float wx4 = mix(w7, w8, tx);
    float wx5 = mix(w9, w10, tx);
    float wx6 = mix(w11, w12, tx);
    float wx7 = mix(w13, w14, tx);
    float wx8 = mix(w15, w16, tx);

    float wy1 = mix(wx1, wx2, ty);
    float wy2 = mix(wx3, wx4, ty);
    float wy3 = mix(wx5, wx6, ty);
    float wy4 = mix(wx7, wx8, ty);

    float wz1 = mix(wy1, wy2, tz);
    float wz2 = mix(wy3, wy4, tz);

    float w = mix(wz1, wz2, tu);

    w = clamp(w, -1.0, 1.0);
    w = (w + 1.0) / 2.0;
    return w;
}

float layered_perlin_noise(
    float x,
    float y,
    float z,
    int n_levels,
    float freq_mult,
    float ampl_mult,
    float freq_init
) {
    float f = 0.0;
    float ampl_sum = 0.0;
    float ampl = 1.0;
    float freq = freq_init;
    for (int i = 0; i < n_levels; ++i) {
        f += ampl * perlin_noise(x * freq, y * freq, z * freq);
        ampl_sum += ampl;
        ampl *= ampl_mult;
        freq *= freq_mult;
    }
    f /= ampl_sum;
    return f;
}

/*
    static.frag
    by Spatial
    05 July 2013
*/

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I[Spatial] whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }

vec2 rotate(vec2 v, float rad) {
    float c = cos(rad);
    float s = sin(rad);
    return vec2(c * v.x - s * v.y, s * v.x + c * v.y);
}

vec2 poisson_disc64(int idx) {
    return POISSON_DISK64[idx];
}
