
template<class model_t>
TetrisBoard<model_t>::TetrisBoard(
    const model_t &model, const TetrisConfig *cfg, wxWindow *parent)
:ha_(0),na_(0),sa_(0),pf_(0)
,wxPanel(parent)
{
    TetrisConfig::fallback_to_default(cfg);

    auto *sizer_next = new wxStaticBoxSizer(wxVERTICAL, this, "Next Area");
    na_ = new NextArea(model, sizer_next->GetStaticBox());
    sizer_next->Add(na_,1,wxALL|wxEXPAND,5);

    auto *sizer_help = new wxStaticBoxSizer(wxVERTICAL, this, "Help");
    auto *help = new HelpArea(model, sizer_help->GetStaticBox());
    sizer_help->Add(help,0,wxALL|wxEXPAND,5);

    auto sizer_hold = new wxStaticBoxSizer(wxVERTICAL, this, "Hold Area");
    ha_ = new HoldArea(model, sizer_hold->GetStaticBox());
    sizer_hold->Add(ha_,1,wxALL|wxEXPAND,5);

    auto sizer_state = new wxStaticBoxSizer(wxVERTICAL, this, "State Area");
    sa_ = new StateArea(model, sizer_state->GetStaticBox());
    sizer_state->Add(sa_,1,wxALL|wxEXPAND,5);

    pf_ = new PlayField(model,cfg,this);

    auto pf_size = pf_->GetMinSize();
    
    auto *lsizer = new wxBoxSizer(wxVERTICAL);
    auto *rsizer = new wxBoxSizer(wxVERTICAL);
    auto *csizer = new wxBoxSizer(wxVERTICAL);
    auto *sizer = new wxBoxSizer(wxHORIZONTAL);

    lsizer->Add(sizer_hold,1,wxALL|wxEXPAND,5);
    lsizer->AddStretchSpacer();
    lsizer->Add(sizer_state,0,wxALL|wxEXPAND,5);
    
    rsizer->Add(sizer_next,1,wxALL|wxEXPAND,5);
    rsizer->Add(sizer_help,0,wxALL|wxEXPAND,5);

    csizer->AddStretchSpacer(1);
    csizer->Add(pf_);
    csizer->AddStretchSpacer(1);

    sizer->Add(lsizer,0,wxALL|wxEXPAND,10);
    sizer->Add(csizer,0,wxALL|wxEXPAND,10);
    sizer->Add(rsizer,0,wxALL|wxEXPAND,10);

    SetSizerAndFit(sizer);
}

static void drawShapeInRect(
    const Shape &shape, wxRect rect, wxGraphicsContext *gc, const TetrisConfig *cfg)
{
    if(!cfg) cfg = &TetrisConfig::make_default();

    if(shape!=Empty) {
        gc->SetPen(wxPen(*wxBLACK,cfg->borderwidth));
        gc->SetBrush(wxBrush(wxColor(cfg->color_of(shape))));
        gc->DrawRectangle(rect.GetX(), rect.GetY(), rect.GetWidth(), rect.GetHeight());
    } 
}

template<class tetromino_t>
static void drawTetroInRect(
    const tetromino_t& tetro, wxRect rect, wxGraphicsContext* gc, const TetrisConfig *cfg)
{
    TetrisConfig::fallback_to_default(cfg);
    int x0 = rect.GetX(), y0 = rect.GetY(), 
        w = rect.GetWidth(), h = rect.GetHeight();

    for(auto& pos: tetro.coord()) {
        auto p = pos - tetro.get_base();
        auto rect_p = wxRect(x0+w*p.x,y0-h*p.y,w,h); // y-axis is downward in device coordinate system
        drawShapeInRect(tetro.get_shape(), rect_p, gc, cfg);
    }
}

template<class tetromino_t>
static void drawTetroInBox(
    const tetromino_t& tetro, wxRect rect, wxGraphicsContext* gc, double scale, const TetrisConfig *cfg)
{
    assert(tetro.coord().size()==4);
    auto s = tetro.get_shape();
    Pos  co[4]; // the coord of normalized tetro in box [0,3]*[0,1] 
    for(int i=0; i<4; i++) co[i] = Pos { 1+raw_pos[s][0][i][0],raw_pos[s][0][i][1] };

    int x0 = rect.GetX(), y0 = rect.GetY(),
        dw = rect.GetWidth()/4.0, dh = rect.GetHeight()/2.0,
        cx = x0+2*dw, cy = y0+dh;
    
    assert( 0 <= scale && scale <= 1.0 );
    if(1.0-scale <= 1e-4) {
        for(int i=0; i<4; i++) {
            auto rect_ij = wxRect(x0+dw*co[i].x,y0+dh*(1-co[i].y),dw,dh);
            drawShapeInRect(s, rect_ij, gc, cfg);
        }
    } else {
        rect = wxRect(cx-2*dw*scale, cy-dh*scale, 4*dw*scale, 2*dh*scale);
        drawTetroInBox(tetro,rect,gc);
    }
}

template<class model_t>
PlayField<model_t>::PlayField(const model_t &model, const TetrisConfig *cfg, wxWindow *parent)
:model_{model},config_{cfg}
,W{model_.size_x()},H{model_.size_y()}
,wxPanel(parent)
{
    TetrisConfig::fallback_to_default(config_);
    SetMinSize(bounding_rect().GetSize());
    SetMaxSize(bounding_rect().GetSize());

    Bind(wxEVT_PAINT, &PlayField::OnPaint, this, wxID_ANY);
}

template<class model_t>
wxRect PlayField<model_t>::bounding_rect() const
{
    int width = W * config_->blocksize,
        height = H * config_->blocksize;
    return wxRect(0,0,width,height);
}

template<class model_t>
Pos PlayField<model_t>::ppos_from_mpos(Pos p) const
{
    return Pos{p.x, static_cast<int>(H)-1-p.y};
}

template<class model_t>
void PlayField<model_t>::OnPaint(wxPaintEvent &e)
{
    wxPaintDC dc(this);
    std::unique_ptr<wxGraphicsContext> gc{ wxGraphicsContext::Create(dc) };
    if(!gc) return;

    // background
    auto rect = bounding_rect();
    
    gc->SetPen(*wxBLACK_PEN);
    gc->DrawRectangle(0,0,rect.GetWidth(),rect.GetHeight());

    // board
    auto bs = config_->blocksize;
    auto rect_of = [bs,this](int i, int j) {
        auto p = ppos_from_mpos({i,j});
        return wxRect(bs * p.x, bs * p.y, bs, bs);
    };
    for(auto i = 0; i < W; i++) for(auto j = 0; j < H; j++)
    {
        auto shape = model_.get_shape_at(i,j);
        drawShapeInRect(shape, rect_of(i,j), gc.get(), config_);
    }

    // tetro
    auto *t = model_.get_tetro();
    auto pos_base = t->get_base();
    drawTetroInRect(*t, rect_of(pos_base.x,pos_base.y), gc.get(), config_);
}

template<class model_t>
HoldArea<model_t>::HoldArea(const model_t &model, wxWindow *parent)
:wxPanel(parent), model_(model)
{
    SetMinSize(wxSize(150,150));
    Bind(wxEVT_PAINT, &HoldArea::OnPaint, this, wxID_ANY);
}

template<class model_t>
void HoldArea<model_t>::OnPaint(wxPaintEvent &e)
{
    wxPaintDC dc(this);
    std::unique_ptr<wxGraphicsContext> gc{wxGraphicsContext::Create(dc)};
    if(!gc) return;

    // make clean 
    wxSize size = GetClientSize();
    int cx = size.GetWidth()/2, 
        cy = size.GetHeight()/2,
        r = std::min(cx,cy);
    gc->SetBrush(*wxWHITE_BRUSH);
    gc->SetPen(*wxTRANSPARENT_PEN);
    gc->DrawEllipse(cx-r,cy-r,2*r,2*r);

    // draw tetro 
    int side = r * 1.414, bs = side / 4.0;

    auto rect = wxRect(cx-2*bs,cy-bs,4*bs,2*bs);
    auto tetro = model_.tetro_from_hold();
    if(tetro) drawTetroInBox(*tetro, rect, gc.get());
}

template<class model_t>
NextArea<model_t>::NextArea(const model_t &model, wxWindow *parent)
:wxPanel(parent), model_(model)
{
    Bind(wxEVT_PAINT, &NextArea::OnPaint, this, wxID_ANY);
}

template<class model_t>
void NextArea<model_t>::OnPaint(wxPaintEvent &e)
{
    wxPaintDC dc(this);
    std::unique_ptr<wxGraphicsContext> gc{wxGraphicsContext::Create(dc)};
    if(!gc) return;

    // make clean
    wxSize size = GetClientSize();
    gc->DrawRectangle(0,0,size.GetWidth(),size.GetHeight());

    // draw tetros 
    int cx0 = size.GetWidth()/2.0, cy0 = size.GetHeight()/2.0/next_size;
    int bs = std::min(cx0/2.0, cy0/1.0);
    for(int i=0;i<next_size;i++) {
        auto *t = model_.tetro_from_next(i+1);
        auto rect_i = wxRect(cx0-2*bs, cy0+i*2*bs-bs, 4*bs, 2*bs);
        drawTetroInBox(*t, rect_i, gc.get(), 0.8);
    }
}

template<class model_t>
StateArea<model_t>::StateArea(const model_t &model, wxWindow *parent)
:wxPanel(parent), model_(model)
{
    SetMinSize(wxSize(100,80));
    Bind(wxEVT_PAINT, &StateArea::OnPaint, this, wxID_ANY);
}

template<class model_t>
void StateArea<model_t>::OnPaint(wxPaintEvent &e)
{
    wxPaintDC dc(this);
    // std::unique_ptr<wxGraphicsContext> gc{wxGraphicsContext::Create(dc)};
    // if(!gc) return;

    auto *st = model_.get_state();

    auto info = wxString::Format(
    "Score:\t%d\n"
    "Level:\t%d\n"
    "Combo:\t%d\n"
    "B2B:  \t%d"
    ,st->get_score(),st->get_level(),st->get_combo(),st->get_back2back()
    );

    dc.DrawLabel(info, GetClientRect());
}

template<class model_t>
HelpArea<model_t>::HelpArea(const model_t&, wxWindow *parent)
:wxPanel(parent)
{
    Bind(wxEVT_PAINT, &HelpArea::OnPaint, this, wxID_ANY);
    SetMinSize(wxSize(180,180));
}

template<class model_t>
void HelpArea<model_t>::OnPaint(wxPaintEvent &e)
{
    wxPaintDC dc(this);
    auto tips = wxString(
    ">>> KEY >>>\n"
    "[⏎]\t\t=>\tNew Game\n"
    "[R]\t\t=>\tRestart\n"
    "[←/→]\t=>\tMove Left/Right\n"
    "[↑]/[X]\t=>\tRotate CW\n"
    "[⌃]/[Z]\t=>\tRotate CCW\n"
    "[↓]\t\t=>\tSoft Drop\n"
    "[␣]\t\t=>\tHard Drop\n"
    "[⇧]/[C]\t=>\tHold\n"
    "[Esc]\t=>\tPause/Resume\n"
    );

    dc.DrawLabel(tips,GetClientRect());
}